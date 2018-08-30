
#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     "string.h"
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
//#define FALSE  -1
//#define TRUE   0
/*********************************************************************/
int OpenDev(char *Dev)
{
  int fd = open( Dev, O_RDWR | O_NOCTTY );         //| O_NOCTTY | O_NDELAY    
  if (-1 == fd)   
  {           
    perror("Can't Open Serial Port");
    return -1;      
  }   
  else    
    return fd;
}
/**
*@brief  ���ô���ͨ������
*@param  fd     ���� int  �򿪴��ڵ��ļ����
*@param  speed  ���� int  �����ٶ�
*@return  void
*/
void set_speed(int fd, int speed)
{
  int   i; 
  int   status; 
  struct termios   Opt;
  
  int speed_arr[] = { B460800, B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B38400, B19200, B9600, B4800, B2400, B1200, B300, };
  int name_arr[] = { 460800, 230400, 115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
19200,  9600, 4800, 2400, 1200,  300, };

  tcgetattr(fd, &Opt); 
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
    if  (speed == name_arr[i]) {     
      tcflush(fd, TCIOFLUSH);     
      cfsetispeed(&Opt, speed_arr[i]);  
      cfsetospeed(&Opt, speed_arr[i]);   
      status = tcsetattr(fd, TCSANOW, &Opt);  
      if  (status != 0) {        
        perror("tcsetattr fd1");  
        return;     
      }    
      tcflush(fd,TCIOFLUSH);   
    }  
  }
}
/**
*@brief   ���ô�������λ��ֹͣλ��Ч��λ
*@param  fd     ����  int  �򿪵Ĵ����ļ����
*@param  databits ����  int ����λ   ȡֵ Ϊ 7 ����8
*@param  stopbits ����  int ֹͣλ   ȡֵΪ 1 ����2
*@param  parity  ����  int  Ч������ ȡֵΪN,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
  struct termios options; 
  options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
  options.c_oflag  &= ~OPOST;   /*Output*/
  if  ( tcgetattr( fd,&options)  !=  0) { 
    perror("SetupSerial 1");     
    return(FALSE);  
  }
  options.c_cflag &= ~CSIZE; 
  switch (databits) /*��������λ��*/
  {   
  case 7:     
    options.c_cflag |= CS7; 
    break;
  case 8:     
    options.c_cflag |= CS8;
    break;   
  default:    
    fprintf(stderr,"Unsupported data size/n"); return (FALSE);  
  }
  switch (parity) 
  {   
  case 'n':
  case 'N':    
    options.c_cflag &= ~PARENB;   /* Clear parity enable */
    options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
    break;  
  case 'o':   
  case 'O':     
    options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/  
    options.c_iflag |= INPCK;             /* Disnable parity checking */ 
    break;  
  case 'e':  
  case 'E':   
    options.c_cflag |= PARENB;     /* Enable parity */    
    options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/     
    options.c_iflag |= INPCK;       /* Disnable parity checking */
    break;
  case 'S': 
  case 's':  /*as no parity*/   
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;break;  
  default:   
    fprintf(stderr,"Unsupported parity/n");    
    return (FALSE);  
  }  
  /* ����ֹͣλ*/  
  switch (stopbits)
  {   
  case 1:    
    options.c_cflag &= ~CSTOPB;  
    break;  
  case 2:    
    options.c_cflag |= CSTOPB;  
    break;
  default:    
    fprintf(stderr,"Unsupported stop bits/n");  
    return (FALSE); 
  } 
  /* Set input parity option */ 
  if (parity != 'n')   
    options.c_iflag |= INPCK; 
  tcflush(fd,TCIFLUSH);
  options.c_cc[VTIME] = 150; /* ���ó�ʱ15 seconds*/   
  options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
  if (tcsetattr(fd,TCSANOW,&options) != 0)   
  { 
    perror("SetupSerial 3");   
    return (FALSE);  
  } 
  return (TRUE);  
}

int OpenDev_2(char *dev) {
  int fd = OpenDev(dev);
  if (-1==fd) {
    printf("OpenDev Error/n");
    exit(0);
    return -1;
  }
  set_speed(fd,115200);
  if (set_Parity(fd,8,1,'N') == FALSE)  
  {
    printf("Set Parity Error/n");
    exit (0);
  }
  return fd;
}
#if 0
int main11(int argc, char **argv)
{
  
  int fd;
  int nread;
  char buff[512];
  char *dev  = "/dev/ttyS0"; //���ڶ�
  fd = OpenDev(dev);
  set_speed(fd,4800);
  if (set_Parity(fd,8,1,'N') == FALSE)  
  {
    printf("Set Parity Error/n");
    exit (0);
  }
  int i;
  i = getchar();
  if ( i == '1')
  {
    while (1) //ѭ����ȡ����
    {   
      while((nread = read(fd, buff, 512))>0)
      { 
        printf("/nLen %d/n",nread); 
        buff[nread+1] = 0;
        printf( "/n%s", buff);   
      }
    }
  }
  if ( i == '2')
  {
    while (1) //ѭ��д������
    {  
      
      gets(buff);
      
      printf("------buff--->%s<--------/n",buff);
      int num = strlen(buff);
      printf("--------num---->%d<--------------/n",num);
      if ( num  > 0)
      {
        printf("Wirte num not NULL./r/n");
        nread = write(fd, buff ,num);
        if(nread == -1)
        {
          printf("Wirte sbuf error./n");
        }
        printf("--nread---->%d<-----------/n",nread);
      }
      
    }
  }
  close(fd); 
  //exit (0);
}

#endif
