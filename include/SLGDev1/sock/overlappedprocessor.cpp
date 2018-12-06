
#include "../slgdev_private.h"
#include "OverlappedProcessor.h"
#include "Socket.h"
#include "SockHandle.h"
#include <assert.h>
#include <mswsock.h>
#ifndef ASSERT
#define ASSERT assert
#endif // ASSERT
#ifdef WIN32

BOOL  sock_wsarecv(SOCKET sock, WSABUF* lpBuffer, ulong* lpNumberOfBytesRecvd, WSAOVERLAPPED* lpOverlapped, ulong& uFlag);
BOOL  sock_wsarecvfrom(SOCKET sock, WSABUF* lpBuffer, ulong* lpNumberOfBytesRecvd, WSAOVERLAPPED* lpOverlapped, ulong& uFlag, sockaddr* pAddr, int32* lpLen);
BOOL  sock_acceptex(SOCKET listen_sock, SOCKET accept_sock, WSABUF* lpBuffer, WSAOVERLAPPED* lpOverlapped);
void  sock_getacceptexsockaddrs(WSABUF* lpBuffer, int8* ip, uint16& port);
typedef struct : public WSAOVERLAPPED
{
    int32 nOperType;
    int32 bListen;
    int32 Param; //���ڵ�bListenΪtureʱ��������
    int32 nQuestBytes;//������ܶ����ֽ�����
    WSABUF WsaBuf;
    void* lpContext;
    CSockBase* user;
    char cRecvBuf[sizeof(addr_in) * 2 + 32]; //�˻�����Ҫ���ڵ�bListenΪtureʱ����Զ��������ַ
} OVERLAPPED_DATA;
BOOL sock_acceptex(SOCKET listen_sock, SOCKET accept_sock, WSABUF* lpBuffer, WSAOVERLAPPED* lpOverlapped)
{
    DWORD dwBytesReceived = 0;
    return AcceptEx(listen_sock, accept_sock, lpBuffer->buf, 0, sizeof(addr_in) + 16,
                    sizeof(addr_in) + 16, &dwBytesReceived, lpOverlapped);
}

void sock_getacceptexsockaddrs(WSABUF* lpBuffer, addr_in* addr)
{
    addr_in* LocalSockaddr = NULL;
    addr_in* RemoteSockaddr = NULL;
    int32 LocalSockaddrLength = sizeof(addr_in);
    int32 RemoteSockaddrLength = sizeof(addr_in);
    GetAcceptExSockaddrs(lpBuffer->buf, 0, sizeof(addr_in) + 16, sizeof(addr_in) + 16, (LPSOCKADDR*)&LocalSockaddr, &RemoteSockaddrLength, (LPSOCKADDR*)&RemoteSockaddr, &RemoteSockaddrLength);

    if (RemoteSockaddr && addr)
    {
        //sa->Port = net_ntop(RemoteSockaddr, sa->IP_Address, IP_LEN);
        *addr = *RemoteSockaddr;
    }

}

COverlappedProcessor::COverlappedProcessor(uint32 uMemPoolSize, uint32 uMemPoolUnit) :
    CNetProcessor(uMemPoolSize, uMemPoolUnit),
    m_bWorking(false),
    m_nTreadCount(0),
    m_hIOCPThread(0)
{
    memset(m_hWorkThread, 0, sizeof(m_hWorkThread));
    m_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //������ɶ˿�
}

COverlappedProcessor::~COverlappedProcessor()
{
    StartUpWork(false);
    CloseHandle(m_hCompletionPort);
}

/////////////////////////////////////////////////////////////////////
//��������: ReadyRecv
//����ʱ��: 2011/07/08 16:57
//�� ��: sock ׼�����ݽ���
// type ���ݽ�������
//�ļ�����: lcy
//����˵��: ���׽��ּ�����ɶ˿ڶ����У��Ա��յ����ݽ�����Ϣ
//////////////////////////////////////////////////////////////////////
BOOL COverlappedProcessor::StartRecv(SOCKET sock , CSockBase* user)
{
    return StartRecv(sock, false, user);
}

BOOL COverlappedProcessor::StartListen(SOCKET sock, CSockBase* user)
{
    return StartRecv(sock, true, user);
}

void COverlappedProcessor::StopRecv(SOCKET sock, CSockBase* user)
{
    // sock_close( sock );
    return ;
}

void COverlappedProcessor::StopListen(SOCKET sock, CSockBase* user)
{
    sock_close(sock);
}

int32 COverlappedProcessor::Send(SOCKET sock, const int8* pbData, int32 nLen, const addr_in* pAddr)
{
    CDataRecvProcess* pObj = NULL;
    m_synSOCKMap.Lock();
    SOCKMap::iterator it = m_SOCKMap.find(sock);

    if (it != m_SOCKMap.end())
    {
        OVERLAPPED_DATA* pData = (OVERLAPPED_DATA*)(it->second);

        if (pData)
        {
            pObj = (CDataRecvProcess*)pData->lpContext;

            if (pObj)
            {
                pObj->AddRef();
            }

        }

    }

    m_synSOCKMap.Unlock();

    if (pObj)
    {
        int32 rs = pObj->Send(pbData, nLen, pAddr);
        pObj->Release();
        return rs;
    }

    return 0;
}

BOOL COverlappedProcessor::StartRecv(SOCKET sock , BOOL bListen, CSockBase* user)
{
    OVERLAPPED_DATA* pOverLapped = NULL;
    m_synSOCKMap.Lock();
    SOCKMap::iterator it = m_SOCKMap.find(sock);

    if (it == m_SOCKMap.end())
    {
        ::CreateIoCompletionPort((HANDLE)sock, m_hCompletionPort, sock, 0);
        pOverLapped = new OVERLAPPED_DATA;
        memset(pOverLapped, 0x0, sizeof(OVERLAPPED_DATA));
        pOverLapped->lpContext = new CDataRecvProcess(sock, m_pThreadPool, user);
        pOverLapped->bListen = bListen;
        pOverLapped->user = user;
        pOverLapped->nOperType = FD_READ;
        m_SOCKMap.insert(std::pair<SOCKET, void*>(sock, pOverLapped));

    }

    m_synSOCKMap.Unlock();

    if (pOverLapped)
    {
        if (SOCK_STREAM == sock_type(sock))
        {
            Recv(sock, pOverLapped, user);
        }

        else if (SOCK_DGRAM == sock_type(sock))
        {
            RecvFrom(sock, pOverLapped, user);
        }

    }

    return 1;
}

/////////////////////////////////////////////////////////////////////
//��������: StartUpWork
//����ʱ��: 2011/07/08 17:05
//�� ��:
//�ļ�����: lcy
//����˵��: ������ֹͣ���ݽ��ܹ�������
//////////////////////////////////////////////////////////////////////
BOOL COverlappedProcessor::StartUpWork(BOOL flag)
{
    if (flag)
    {
        if (!m_bWorking)
        {
            m_bWorking = true;
#if 1
            SYSTEM_INFO info = { 0 };
            ::GetSystemInfo(&info);
            m_nTreadCount = info.dwNumberOfProcessors + 2;

            if (m_nTreadCount > MAX_TREADS)
            {
                m_nTreadCount = MAX_TREADS;
            }

            for (int i = 0; i < m_nTreadCount; ++i)
            {
                if (NULL == m_hWorkThread[i])
                {
                    m_hWorkThread[i] = (HANDLE)CreateThread(0, 0, COverlappedProcessor::process1, this, 0, 0);
                }

            }

#else
            m_hIOCPThread = (HANDLE)_beginthread(COverlappedProcessor::process1, 0, this);
#endif
        }

    }

    else
    {
        if (m_bWorking)
        {
            m_bWorking = false;
#if 1
            int i;

            for (i = 0; i < m_nTreadCount; ++i)
            {
                //Ͷ����ɶ˿���Ϣ�������̳߳�
                ::PostQueuedCompletionStatus(m_hCompletionPort, -1, 0, NULL);
            }

            // ::WaitForMultipleObjects( m_nTreadCount, m_hWorkThread, TRUE, 1000*TIME_OUT);
            for (i = 0; i < m_nTreadCount; ++i)
            {
                if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hWorkThread[i], 1000))
                {
                    ::TerminateThread(m_hWorkThread[i], 0);
                }

                m_hWorkThread[i] = NULL;
            }

#else
            ::PostQueuedCompletionStatus(m_hCompletionPort, -1, 0, NULL);
            ::PostQueuedCompletionStatus(m_hCompletionPort, -1, 0, NULL);

            if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hIOCPThread, 1000 * TIME_OUT))
            {
                ::TerminateThread(m_hIOCPThread, 0);
            }

            m_hIOCPThread = NULL;
#endif
        }

    }

    return true;
}

unsigned long WINAPI COverlappedProcessor::process1(void* param)
{
    COverlappedProcessor* pthis = (COverlappedProcessor*)param;

    if (pthis)
    {
        pthis->process();
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////
//��������: process
//����ʱ��: 2011/07/08 17:06
//�� ��:
//�ļ�����: lcy
//����˵��: �̴߳�����
//////////////////////////////////////////////////////////////////////
void COverlappedProcessor::process()
{
    DWORD dwTrans = 0;
    DWORD* dkey = 0;
    LPOVERLAPPED lpover = NULL;
    BOOL bok = false;

    while (m_bWorking)
    {
        dwTrans = 0;
        dkey = 0;
        lpover = NULL;
        bok = FALSE;
        SetLastError(0);
        bok = ::GetQueuedCompletionStatus(m_hCompletionPort, &dwTrans, (PULONG_PTR)&dkey, &lpover, INFINITE);
        OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)lpover;

        if (!ErrDeal((SOCKET)dkey, lpover, dwTrans, bok, lpOverlapped ? lpOverlapped->user : NULL))
        {
            continue;
        }

        if (!(lpOverlapped->bListen))
        {
            HandleIO((SOCKET)dkey, (WSAOVERLAPPED*)lpover, dwTrans, 1, lpOverlapped ? lpOverlapped->user : NULL);
        }

        else
        {
            HandleListen((SOCKET)dkey, (WSAOVERLAPPED*)lpover, dwTrans, 1, lpOverlapped ? lpOverlapped->user : NULL);
        }

    }

}

BOOL COverlappedProcessor::ErrDeal(SOCKET sock, WSAOVERLAPPED* pIOOper, DWORD dwTrans, BOOL bOK, CSockBase* user)
{
    int nError = sock_error();

    if (sock == INVALID_SOCKET || NULL == pIOOper)
    {
        return false;
    }

    if (dwTrans == -1)
    {
        m_bWorking = false;
        return false;
    }

    OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)pIOOper;

    if ((!bOK && dwTrans == 0 && !(lpOverlapped->bListen)) ||
            (!bOK && (nError == WSAENETRESET || nError == WSAECONNABORTED || nError == WSAECONNRESET))
       )
    {
        //DEBUG_INFO5("bOK = %d lpOverlapped = %08x, dwTrans=%d nError = %d %s\n", bOK, lpOverlapped, dwTrans, nError, strerror(nError));
        sock_close(sock);
        BOOL bExit = false;
        m_synSOCKMap.Lock();
        SOCKMap::iterator it = m_SOCKMap.find(sock);

        if (it != m_SOCKMap.end())
        {
            m_SOCKMap.erase(it);
            bExit = true;
        }

        m_synSOCKMap.Unlock();

        if (bExit)
        {
            CDataRecvProcess* pObj = (CDataRecvProcess*)(lpOverlapped->lpContext);
            lpOverlapped->lpContext = NULL;
            pObj->Release();
            DEBUG_INFO("__FILE__ %s __LINE__ %d\n", __FILE__, __LINE__);
            delete lpOverlapped;
            CNetProcessor::StopRecv(sock, user);
        }

        return false;
    }

    return true;
}

void COverlappedProcessor::HandleListen(SOCKET sock, WSAOVERLAPPED* pIOOper, DWORD dwTrans, int nError, CSockBase* user)
{
    OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)pIOOper;

    if (FD_READ == lpOverlapped->nOperType)   //���ܵ�����
    {
        addr_in addr[1] = { 0 };
        sock_getacceptexsockaddrs(&(lpOverlapped->WsaBuf), addr);
        CNetProcessor::Accept(lpOverlapped->Param, addr, user);
        Recv(sock, pIOOper, user);
    }

}

/////////////////////////////////////////////////////////////////////
//��������: HandleIO
//����ʱ��: 2011/07/08 17:09
//�� ��: sock �׽���
// pIOOper ��ɶ˿�WSAOVERLAPPED�ṹ�����Ͳ���
// dwTrans ���ݽ��ܴ�С
// nError ����ֵ
//�ļ�����: lcy
//����˵��: ���ݽ��ܴ���
//////////////////////////////////////////////////////////////////////
void COverlappedProcessor::HandleIO(SOCKET sock, WSAOVERLAPPED* pIOOper , DWORD dwTrans, int nError, CSockBase* user)
{
    OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)pIOOper;

    if (FD_READ == lpOverlapped->nOperType)   //���ܵ�����
    {
        CDataRecvProcess* pCurPrecessor = (CDataRecvProcess*)(lpOverlapped->lpContext);

        //printf("SOCKET=%d\n", sock);
        if (SOCK_STREAM == sock_type(sock))
        {
            if (pCurPrecessor->Recv())
            {
                pCurPrecessor->CheckTimeOut(RECV_TIME_OUT);
                Recv(sock, pIOOper, user);
            }

            else
            {
                SetLastError(WSAECONNABORTED);
                ErrDeal(sock, pIOOper, 0, FALSE, user);
            }

        }

        else
        {
            pCurPrecessor->RecvFrom();
            pCurPrecessor->CheckTimeOut(RECV_TIME_OUT);
            RecvFrom(sock, pIOOper, user);
        }

    }

}

/////////////////////////////////////////////////////////////////////
//��������: Recv
//����ʱ��: 2011/07/08 17:11
//�� ��: sock �׽���
//pIOOper ��ɶ˿�WSAOVERLAPPED�ṹ�����Ͳ���
//�ļ�����: lcy
//����˵��: ����һ����ȡ����
//////////////////////////////////////////////////////////////////////
void COverlappedProcessor::Recv(SOCKET sock, WSAOVERLAPPED* pIOOper, CSockBase* user)
{
    OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)pIOOper;
    ulong nRecvBytes = sizeof(SOCK_DATA_HEADER);
    lpOverlapped->WsaBuf.buf = lpOverlapped->cRecvBuf;
    lpOverlapped->WsaBuf.len = nRecvBytes;
    BOOL bok = true;

    if (lpOverlapped->bListen)
    {
        memset(lpOverlapped->cRecvBuf, 0x0, sizeof(lpOverlapped->cRecvBuf));
        ASSERT(0);
        lpOverlapped->Param = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
        bok = sock_acceptex(sock, lpOverlapped->Param, &(lpOverlapped->WsaBuf), pIOOper);
    }

    else
    {
        ulong uFlag = MSG_PEEK;//����ʹ�ô˱����Ϊ�˻�ȡ�׽��������ݽ���֪ͨ
        //��������������,����������������CDataRecvProcess����
        bok = sock_wsarecv(sock, &(lpOverlapped->WsaBuf), &nRecvBytes, lpOverlapped, uFlag);
    }

    ErrDeal(sock, pIOOper, nRecvBytes, bok, user);
}

void COverlappedProcessor::RecvFrom(SOCKET sock, WSAOVERLAPPED* pIOOper, CSockBase* user)
{
    OVERLAPPED_DATA* lpOverlapped = (OVERLAPPED_DATA*)pIOOper;
    lpOverlapped->WsaBuf.buf = lpOverlapped->cRecvBuf ;
    ulong nRecvBytes = sizeof(SOCK_DATA_HEADER);
    lpOverlapped->WsaBuf.len = nRecvBytes;
    ulong uFlag = MSG_PEEK;//����ʹ�ô˱����Ϊ�˻�ȡ�׽��������ݽ���֪ͨ
    //��������������,����������������CDataRecvProcess����
    sock_wsarecvfrom(sock, &(lpOverlapped->WsaBuf), &nRecvBytes, lpOverlapped, uFlag, NULL, NULL);
}

#endif

