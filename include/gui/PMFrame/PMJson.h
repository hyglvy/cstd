//********************************************************************************
//�ļ���PMJSON.h
//���ߣ�feiren
//ʱ�䣺2011-8-20
//��;����װJson�Ļ�������
//�汾��1.0.0.0
//��ϵ��feirench@gmail.com
//********************************************************************************
#ifndef __PMJSON_H__
#define __PMJSON_H__

#include "PMBaseObject.h"
#include "PMInclude.h"
#include <windows.h>
#include <ShellAPI.h>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

#pragma warning (disable : 4996)

class TJSON;
#define PreMallocSize 120*1024
#define PreMallocRatio 5

typedef map<int,TJSON*> TINDEXJSON; 
typedef map<string,TJSON*> TNAMEJSON; 
typedef vector<TJSON*> TJSONS; 
enum ValueType
{
	nullValue = 0, ///< 'null' value
	intValue,      ///< signed integer value
	uintValue,	   ///< unsigned integer value
	int64Value,    ///< long long value
	realValue,     ///< double value
	stringValue,   ///< UTF-8 string value
	booleanValue,  ///< bool value
	pointerValue,  ///< pointer value
	indexValue,	   ///< index value
	binaryValue,   ///< binary value---->len,pdata
	bFileValue,    ///< binary file value---->len,position
	wellValue,	   ///< well value---->char*,pointer
	arrayValue,    ///< array value (ordered list)
	objectValue    ///< object value (collection of name/value pairs).
};
struct TBuffer 
{	
	TBuffer():size(0),useLen(0),buffer(NULL){}
	TBuffer(unsigned int presize):size(presize),useLen(0){buffer=(char*)malloc(presize);memset(buffer,0,presize);}
	void Initialize(){useLen=0;size=PreMallocSize;buffer=(char*)malloc(PreMallocSize);memset(buffer,0,PreMallocSize);}
	void Add(const char *data,int len);
	void operator +=(const char *data)
	{
		Add(data,strlen(data));
	}
	void operator +=(unsigned long data)
	{
		char buf[32]={0};
		ultoa(data,buf,10);
		Add(buf,strlen(buf));
	}
	void operator +=(unsigned int data)
	{
		char buf[32]={0};
		ultoa(data,buf,10);
		Add(buf,strlen(buf));
	}
	void Clear();	
	void MemoryReduce();	
	bool Write(const char *filename);
	bool Read(const char *filename);
	char *buffer;
	unsigned int size;
	unsigned int useLen;
};

class TJSONIterator
{
public:	
	TJSONIterator();
	TJSONIterator(const TNAMEJSON::iterator &state );	
	TJSONIterator(const TJSONS::iterator &state );	
	TJSONIterator(const TJSONIterator &other);
	TJSONIterator& operator =(const TJSONIterator &other);	
	bool operator ==(const TJSONIterator &other);	
	bool operator !=(const TJSONIterator &other);
	bool operator >(const TJSONIterator &other);	
	bool operator <(const TJSONIterator &other);
	bool operator >=(const TJSONIterator &other);	
	bool operator <=(const TJSONIterator &other);
	int operator -(const TJSONIterator &other);
	TJSON& operator *();
	TJSON* operator ->();
	TJSONIterator& operator ++();
	TJSONIterator& operator --();
	TJSONIterator operator ++(int);
	TJSONIterator operator --(int);
	TJSONIterator operator +(int distance);
	TJSONIterator operator -(int distance);

	string GetName();
private:
	TJSONS::iterator array_;
	TNAMEJSON::iterator map_;
	char isArray_;
};

class TJSONConstIterator
{
public:	
	TJSONConstIterator();
	TJSONConstIterator(const TNAMEJSON::const_iterator &state );	
	TJSONConstIterator(const TJSONS::const_iterator &state );	
	TJSONConstIterator(const TJSONConstIterator &other);
	TJSONConstIterator& operator =(const TJSONConstIterator &other);	
	bool operator ==(const TJSONConstIterator &other);	
	bool operator !=(const TJSONConstIterator &other);
	bool operator >(const TJSONConstIterator &other);	
	bool operator <(const TJSONConstIterator &other);
	bool operator >=(const TJSONConstIterator &other);	
	bool operator <=(const TJSONConstIterator &other);
	int operator -(const TJSONConstIterator &other);
	const TJSON& operator *();
	const TJSON* operator ->();
	TJSONConstIterator& operator ++();
	TJSONConstIterator& operator --();
	TJSONConstIterator operator ++(int);
	TJSONConstIterator operator --(int);
	TJSONConstIterator operator +(int distance);
	TJSONConstIterator operator -(int distance);
	string GetName();
private:
	TJSONS::const_iterator array_;
	TNAMEJSON::const_iterator map_;
	char isArray_;
};


class TJSON
{
	static char *NewString(const char *data);
	static TNAMEJSON *NewMap();			
	static TJSONS *NewArray();	
	static TJSON *NewJSON();
	void DeleteString();
	void DeleteMap();			
	void DeleteArray();	
	void DeleteBinary();
	
public:
	typedef TJSONIterator iterator;
	typedef TJSONConstIterator const_iterator;
	typedef std::vector<std::string> Members;
	static const TJSON null;
	static const int minInt;
	static const int maxInt;
	static const unsigned int maxUInt;
	static TJSON* Create();
	static TJSON* Create(bool data);
	static TJSON* Create(int data);
	static TJSON* Create(unsigned int data);
	static TJSON* Create(INT64 data);
	static TJSON* Create(double data);
	static TJSON* Create(const char* data);
	static TJSON* Create(const string &data);
	static TJSON* Create(void *data);
	static TJSON* Create(const TJSON &data);
	static void   DeleteJSON(TJSON *json);	
	static TJSON* ReadFile(const char *fileName);

	TJSON();
	TJSON(bool data);
	TJSON(int data);
	TJSON(unsigned int data);
	TJSON(INT64 data);
	TJSON(double data);
	TJSON(const char* data);
	TJSON(const string &data);
	TJSON(void *data);
	TJSON(const TJSON &data);
	TJSON(TBuffer &data);
	virtual ~TJSON();
	TJSON &operator=( bool data );
	TJSON &operator=( int data );
	TJSON &operator=( unsigned int data );
	TJSON &operator=( INT64 data );
	TJSON &operator=( double data );
	TJSON &operator=( const char* data );
	TJSON &operator=( const string &data );
	TJSON &operator=( void *data );
	TJSON &operator=( const TJSON &data );
	TJSON &operator=( TBuffer &data );

	bool operator==( bool data )const;
	bool operator==( int data )const;
	bool operator==( unsigned int data )const;
	bool operator==( INT64 data )const;
	bool operator==( double data )const;
	bool operator==( const char* data )const;
	bool operator==( const string &data )const;
	bool operator==( void *data )const;
	bool operator==( const TJSON &other )const;

	bool operator!=( bool data )const;
	bool operator!=( int data )const;
	bool operator!=( unsigned int data )const;
	bool operator!=( INT64 data )const;
	bool operator!=( double data )const;
	bool operator!=( const char* data )const;
	bool operator!=( const string &data )const;
	bool operator!=( void *data )const;
	bool operator!=( const TJSON &other )const;

	bool operator<=( int data )const;
	bool operator<=( unsigned int data )const;
	bool operator<=( INT64 data )const;
	bool operator<=( double data )const;
	bool operator<=( const char* data )const;
	bool operator<=( const string &data )const;

	bool operator>=( int data )const;
	bool operator>=( unsigned int data )const;
	bool operator>=( INT64 data )const;
	bool operator>=( double data )const;
	bool operator>=( const char* data )const;
	bool operator>=( const string &data )const;

	bool operator<( int data )const;
	bool operator<( unsigned int data )const;
	bool operator<( INT64 data )const;
	bool operator<( double data )const;
	bool operator<( const char* data )const;
	bool operator<( const string &data )const;

	bool operator>( int data )const;
	bool operator>( unsigned int data )const;
	bool operator>( INT64 data )const;
	bool operator>( double data )const;
	bool operator>( const char* data )const;
	bool operator>( const string &data )const;

	bool operator <( const TJSON &other ) const;
	bool operator <=( const TJSON &other ) const;
	bool operator >=( const TJSON &other ) const;
	bool operator >( const TJSON &other ) const;

	ValueType GetType()const;										//��ȡ����	
	bool	Equal(TJSON *data)const;											//�ж��Ƿ���Ч
	bool	IsNull()const;											//�ж��Ƿ���Ч
	bool	IsBool()const;											//�ж��Ƿ�bool����
	bool	IsInt()const;											//�ж��Ƿ�int����
	bool	IsUInt()const;											//�ж��Ƿ�UInt����
	bool	IsInt64()const;											//�ж��Ƿ�INT64����
	bool	IsDouble()const;										//�ж��Ƿ�double����
	bool	IsString()const;										//�ж��Ƿ�char*����
	bool	IsPointer()const;										//�ж��Ƿ�void*����	
	bool	IsArray()const;											//�ж��Ƿ�������
	bool	IsObject()const;										//�ж��Ƿ��Ƕ���
	bool	IsBinary()const;										//�ж��Ƿ��Ƕ���������
	bool	ToBool()const;											//ת����bool����
	bool	IsParticular()const;									//�Ƿ���������-0
	void	SetParticular();										//����-0
	int     ToInt()const;											//ת����int����	
	unsigned int ToUInt()const;										//ת����UInt����
	INT64   ToInt64()const;											//ת����INT64����
	double  ToDouble()const;										//ת����double����
	char*   ToCString()const;										//ת����char*����
	string  ToString()const;										//ת����string����
	void*   ToPointer()const;										//ת����void*����
	TBuffer ToBinary()const;										//ת���ɶ���������
	void    SetDefine(void *data);									//��ȡvoid*���͵Ķ���,����double��INT64�������ͣ��������Ͷ����Դ洢����
	void*   GetDefine()const;										//��ȡvoid*���͵Ķ���,����double��INT64�������ͣ��������Ͷ����Դ洢����
	string  GetName()const;											//��ȡ����
	int     GetChildCount()const;									//��ȡ���Ӹ���
	void    SetChildCount(int n);									//���ú��Ӹ�����jsonֻ��������
	TJSON * GetChild()const;										//��ȡ��ȡ��һ������
	TJSON * GetLastChild()const;									//��ȡ��ȡ���һ������
	TJSON * GetFather()const;										//��ȡ�丸�׽ڵ�
	bool    Empty()const;											//�ж��ӽڵ��Ƿ�Ϊ��
	void    SetObject();											//���ڵ�����Ϊ����			
	void    SetArray();												//���ڵ�����Ϊ����
	void    SetBinary(const char *value,unsigned int length);		//���ڵ�����Ϊ����������
	//�߼�����
	bool    IsMember( const char *key ) const;						//�ж��Ƿ��ǳ�Ա
	bool    IsMember( const std::string &key ) const;				//�ж��Ƿ��ǳ�Ա
	TJSON * Clone(bool bNodeOnly=false);							//���˽ڵ���������Ϊ��һ����
	void    DeleteItem(unsigned int index,bool bfree=true);			//ɾ���ӽڵ�����bfree�����Ƿ��ͷ��ڴ�
	void    DeleteItem(const char *key,bool bfree=true);			//ɾ���ӽڵ�����bfree�����Ƿ��ͷ��ڴ�
	void    DeleteItem(const string &key,bool bfree=true);			//ɾ���ӽڵ�����bfree�����Ƿ��ͷ��ڴ�
	void    DeleteItem(TJSON *item,bool bfree=true);				//ɾ���ӽڵ�����bfree�����Ƿ��ͷ��ڴ�
	TJSON& operator [](unsigned int index);							//���������ڵĶ�������������򴴽��¶�����ӵ��������	
	TJSON& operator [](const string &key);							//���������ڵĶ�������������򴴽��¶�����ӵ��������
	const TJSON& operator [](unsigned int index)const;				//���������ڵĶ���	
	const TJSON& operator [](const string &key)const;				//���������ڵĶ���
	TJSON&			At( unsigned int index );						//���������ڵĶ�������������򴴽��¶�����ӵ��������
	const TJSON&	At( unsigned int index )const;					//���������ڵĶ���
	TJSON&			At( const string &key );						//���������ڵĶ�������������򴴽��¶�����ӵ��������
	const TJSON&	At( const string &key )const;					//���������ڵĶ���
	TRect<int>		GetRect(unsigned int index=0)const;				//��ȡ����
	TPoint<int>		GetPoint(unsigned int index=0)const;			//��ȡ��
	TSize<int>		GetSize(unsigned int index=0)const;				//��ȡ��
	TJSON*          Find(const char *key);							//���Ҷ����Ա
	TJSON*          Find(const string &key);						//���Ҷ����Ա
	const TJSON*    Find(const char *key)const;						//���Ҷ����Ա
	const TJSON*    Find(const string &key)const;					//���Ҷ����Ա
	Members         GetMemberNames() const;							//��ȡ���е��Ӷ�������
	iterator        begin();										//��ȡ��������Ŀ�ʼ������
	iterator        end();											//��ȡ��������Ľ���������
	iterator        GetIterator();
	const_iterator  GetIterator()const;
	const_iterator  begin()const;									//��ȡ��������Ŀ�ʼ������
	const_iterator  end()const;										//��ȡ��������Ľ���������
	bool		 Replace(unsigned int index,TJSON *data,bool bInsert=false);//�滻ָ������bInsert��ʾ�����������		
	bool		 Replace(const char *name,TJSON *data,bool bInsert=false);	//�滻ָ������bInsert��ʾ�����������
	bool		 Replace(const string &name,TJSON *data,bool bInsert=false);//�滻ָ������bInsert��ʾ�����������
	void		 RangeReplace(TJSON &jnRange,TJSON &jnNew);					//ֻ����������������滻
	void		 RangeReplace(int iFrom,TJSON &jnOld,TJSON &jnNew);		//ֻ����������������滻
	void		 RangeReplace(int iFrom,int iTo,TJSON &jnNew);				//ֻ����������������滻
	void		 RangeReplace(int iPos,TJSON &jnNew);						//ֻ����������������滻
	void		 RangeReplace(TJSON *jnPosition,TJSON &jnNew);				//ֻ����������������滻
	void		 RangeInsert(int iPos,TJSON &jnNew);						//ֻ����������Ĳ���
	void		 RangeInsert(TJSON *jnPosition,TJSON &jnNew,bool back=true);//ֻ����������Ĳ���
	void		 RangeInsert(TJSON &jnNew,TJSON *jnFrom,TJSON *jnTo);		//ֻ����������Ĳ���
	void		 RangeInsert(TJSON &jnNew);									//ֻ����������Ĳ���
	void		 RangeDelete(TJSON &jnNew);									//ֻ���������������ɾ��
	void		 RangeDelete(int iPos,int nCount);							//ֻ���������������ɾ��
	void		 RangeCopy(TJSON &jnRange,int iFrom,int iTo=-1);			//ֻ�������������������
	void		 RangeCopy(TJSON &jnRange);									//ֻ�������������������
	void		 Clear();													//��json����ɾ������գ���ɿն���
	void		 RemoveAll();												//��json�����Ƴ�
	void		 Rename(const string &name);								//������
	TJSON *		 NewArraryObject();
	//�������ӡ�ɾ������
	TJSON*       Push( int data );									//��������������
	TJSON*		 Push( unsigned int data );							//��������������
	TJSON*		 Push( INT64 data );								//��������������
	TJSON*		 Push( double data );								//��������������
	TJSON*		 Push( const char* data );							//��������������
	TJSON*		 Push( const string &data );						//��������������
	TJSON*		 Push( const TJSON &data );							//��������������			
	TJSON*		 Push( TBuffer &data );								//��������������		
	TJSON*		 Front();											//��ȡ�����һ���ӽڵ�
	TJSON*		 Back();											//��ȡ�������һ���ӽڵ�
	const TJSON* Front()const;										//��ȡ�����һ���ӽڵ�
	const TJSON* Back()const;										//��ȡ�������һ���ӽڵ�
	void         Pop(bool bfree=true);								//ɾ���������һ���ڵ㣬bfree�����Ƿ��ͷ�
	void         DeleteRange(TJSON &jnRange);						//ɾ�������һ������
	//�ַ�������
	void		 FromString(const char *data);
	TBuffer      Print(bool format)const;							//��jsonת�����ַ���,bFormat��ʾ�Ƿ��ʽ��	
	void         parse( std::istream& sin);							//�����������json							
	void         parse( const char *Data);							//���ַ����������json
	void         Parse(const std::string &document);				//���ַ����������json
	void         Read(const char *fileName);						//���ļ������json
	void         Write(const char *fileName,bool bFormat=false)const;//��jsonд���ļ���,bFormat��ʾ�Ƿ��ʽ��

	TINDEXJSON & GetIndexs();
	void         Push(int index , TJSON *json);
	void         Push( TJSON *data );								//��������������
	TJSON&       Push();											//��������������
	void		 Push( TRect<int> &data );							//��������������
	void		 Insert(int index , TJSON *json);					//��������
	void		 Insert(const string &name , TJSON *json);				//��������
private:

	const char *parse_value(const char *value);						//��value�������json
	const char *parse_object(const char *value);					//��������
	const char *parse_array(const char *value);						//��������
	const char *parse_binary(const char *value);					//��������������
	const char *parse_binary_file(const char *value);				//�����������ļ�
	const char *parse_well(const char *value);						//����Ƕ�׵�json����
	const char *parse_string(const char *str);						//�����ַ���
	const char *parse_string(const char *str,char* &name);			//�����ַ���
	const char *parse_number(const char *num);						//��������
	const char *skip(const char *in);								//���Կ��ַ�
	bool        IsComment(const char *in);
	const char *RemoveComment(const char *in);
	bool        print_value(TBuffer &out,int depth,int fmt)const;	//��json���л�
	bool        print_string(TBuffer &out)const;					//��ӡ�ַ���
	char *      print_string_ptr(const char *str)const;				//��ӡ�ַ���
	bool        print_binary(TBuffer &out)const;					//��ӡ����������
	bool        print_array(TBuffer &out,int depth,int fmt)const;	//��ӡ����
	bool        print_object(TBuffer &out,int depth,int fmt)const;	//��ӡ����
	bool        print_number(TBuffer &out)const;					//��ӡ����
private:
	const char * getNumber(const char *value,unsigned int &number);	
private:
	union ValueHolder
	{
		INT64 int64;												//�洢64λ����
		double real_;												//�洢double����
		struct BData												//�洢����������,ֻ�洢С��10M������				
		{
			unsigned int length_;									//���ݳ���
			char *define_;											//�洢����			
		}bdata_;	
		struct FData 												//�洢����������,ֻ�洢����10M������	
		{
			unsigned int length_;									//���ݳ���
			unsigned int position_;									//���ݵ�λ��			
		}fdata_;
		struct WData 					
		{
			char *file_;											//json�ļ�����
			TJSON *define_;											//�洢�򿪵�json			
		}wdata_;	
		struct SingleData 					
		{
			union Single
			{
				int int_;											//�洢int����
				unsigned int uint_;									//�洢UInt����
				bool bool_;											//�洢bool����
				char *string_;										//�洢char*����
				TNAMEJSON *map_;									//�洢object����
				TINDEXJSON *imap_;									//�洢�����������������
				TJSONS *array_;										//�洢array����
				void *pointer_;										//�洢ָ������
			}data_;
			void *define_;											//�洢��������			
		}data_;		
	} value_;
	TNAMEJSON::iterator iter;										//�洢����������,�����ȡ���ڵ������
	/*TJSONS::iterator */
	TJSON *father;													//�洢���׽ڵ�
	ValueType type_ : 8;											//�洢��ڵ�����	
};

#endif