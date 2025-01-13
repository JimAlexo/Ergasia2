#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "Item.h"

//ALEXOPOULOS DIMITRIS AM:2123043
//QEFALIJA LUIZ AM:2123289
/*
typedef struct Item{
  char description[50];
  float price;
  int item_count;
  int want;
  int A1[5];
}Item;
*/

int main(int argc , char* argv[]){

  srand(time(NULL)+getpid());
  
  int fd1[2];
  int fd2[2];
  int fd3[2];

  if (pipe(fd1)==-1){
    printf("ERROR occured while pipeing \n");
    return 1;
  }
  if (pipe(fd2)==-1){
    printf("ERROR occured while pipeing \n");
    return 1;
  }
  if (pipe(fd3)==-1){
    printf("ERROR occured while pipeing \n");
    return 1;
  }
  int id = fork();
  if (id==-1){
    printf("ERROR occured while forking \n");
    return 1;
  }else if(id==0){/////// CHILD PROCESSES ///////////////////////////////////////////////////////////////////////////////////////////////

    close (fd1[1]);
    close (fd2[1]);
    close (fd3[0]);
    
    int z;//z(Epilogi mesw rand)
    int n;//Megethos string pou tha kanw read
    char str[100];//To string pou tha kanw read
    
    for (int i=0;i<5;i++){//Epanalipsi gia tin dhmiourigia 5 Clients
      int id2=fork();
      if (id2==-1){
        printf("ERROR occured while forking \n");
        return 1;
      }else if(id2==0){
        for(int j=0;j<10;j++){//Epanalipsi gia 10 paragelies
          z=rand()%20;//Epilogh tuxaiou arithmou
          write(fd3[1],&z,sizeof(int));//Apostolh tou tuxaiou arithmou(item paragelias)
        }
        read(fd1[0],&n,sizeof(int));
        read(fd2[0],str,sizeof(char)*n);//Paralavi munimatwn gia emfanisi
        printf("%s",str);
        sleep(1);//Anamoni enos sec
        
        close (fd1[0]);
        close (fd2[0]);
        close (fd3[1]);
        return 0;
        
      }else{
        wait(NULL);
      }
    }
    close (fd1[0]);
    close (fd2[0]);
    close (fd3[1]);
    return 0;
    
  }else{///////////////// PARENT PROCESS ////////////////////////////////////////////////////////////////////////////////////////////////

    close (fd1[0]);
    close (fd2[0]);
    close (fd3[1]);
    
    int p1=0,p2=0,p3=0,p4=0;// p1(epituxeis pwlhseis) p2(apotiximenes pwlhseis) p3(zitoumena proionta) p4(prwionta pou poulithikan)
    float S=0;//S(tziros)
    Item catalogue[20];
    float x;//Gia to diavasma tou price
    for (int i=0;i<20;i++){//Epanalipsi gia ola ta items
      printf("Name the item you want to list and then give its according price: \n");
      scanf("%s %f",catalogue[i].description,&x);//Diavazw onoma kai price
      while (x<=0){//Elegxos orthotitas gia to price
        printf("Price cant be less or equal to zero, please give a valid price: \n");
        scanf("%f",&x);
      }
      catalogue[i].price=x;//Timh
      catalogue[i].item_count=2;//Apothema
      catalogue[i].want=0;//Posoi thelisan afto to proion
      for (int j=0;j<5;j++){
        catalogue[i].A1[j]=0;//Pinakas gia poious to poioi pelates thelisan to prwion kai den to piran
      }
      printf("\n");//Gia omorfia
    }
    int k;//Epilogi proiodos apo random
    for (int i=0;i<5;i++){//5 Epanalipsis, 1 gia kathe pelati
      int fail=0;//fail(Elegxos gia to an h sunalagi apetuxe)
      float s1=0;//s1(Tziros pelati)
      int A[10];//Gia na krataw ta proionta se periptwsh akurwshs ths paragelias
      char str[100];
      for (int j=0;j<10;j++){//10 Epanalipsis gia tis agores tou pelati
        read(fd3[0],&k,sizeof(int));
        A[j]=-1;
        if (catalogue[k].item_count>0){//Periptwsh pou uparxei se diathesimotita
          s1+=catalogue[k].price;//Afksisi tou tzirou tou sigekrimenou pelati
          catalogue[k].item_count--;//Afereite to item apo to stock
          catalogue[k].want++;//+1 epithimies proiontos
          p3++;
          p4++;
          A[j]=k;
        }else{//Periptwsh pou to antikeimeno eksantlithike
          fail=1;//Deixnw oti apetixe h sunalagi
          p2++;
          p3++;
          p4-=j;//Epanafora proiontwn pou poulithikan
          catalogue[k].want++;//+1 epithimies proiontos
          catalogue[k].A1[i]++;//Afksisi tou pinaka pou metraei posoi thelisan to proion kai den to piran
          for (int l=0;l<j;l++){
            catalogue[A[l]].item_count++;//Epistrofi ton prwiontwn meta apo akurwsh paragelias
          }
          sprintf(str,"Client %d : Products unavailable, request failed. \n \n",i+1);
          int n= strlen(str)+1;
          write(fd1[1],&n,sizeof(int));//Apostolh munimatos me to megethos tou string
          write(fd2[1],str,sizeof(char) * n);//Apostolh tou string
          break;
        }
      }
      if (fail==0){
        p1++;
        S+=s1;
        sprintf(str,"Client %d : Purchase complete, your total sum is: %.2f euro. \n \n",i+1,s1);
        int n= strlen(str)+1;
        write(fd1[1],&n,sizeof(int));//Apostolh munimatos me to megethos tou string
        write(fd2[1],str,sizeof(char) * n);//Apostolh tou string
      }
    }
    wait(NULL);//Perimenw na termatisi kai o teleftaios pelatis
    int r;
    for (int i=0;i<20;i++){
      printf("\nItem Name: %s \n",catalogue[i].description);//Perigrafi tou proiontos
      printf("This item was requested %d times. \n",catalogue[i].want);//Arithmos etimatwn gia agora
      r=((catalogue[i].item_count-2)*(-1));
      printf("%d units were sold. \n",r);//Arithmos temaxiwn pou poulithikan
      if ((catalogue[i].want>2)&&(catalogue[i].item_count==0)){
        printf("And the following clients weren't able to get this item :");
        for (int j=0;j<5;j++){
          if(catalogue[i].A1[j]==1){
            printf("Client %d ",j+1);//Lista pelatwn pou to thelisan kai den prolavan na to paroun epidi teleiwse
          }
        }
        printf("\n");//Allagh gramhs gia na einai meta pio omorfo
      }
    }
    printf("\n");
    printf("5 requests were made, where %d succeeded and %d failed \n",p1,p2);//Emfanisi genikou minimatos
    printf("%d products were requested, where %d products were bought for a total of %.2f \n",p3,p4,S);//Emfanisi genikou minimatos
  }
  close (fd1[1]);
  close (fd2[1]);
  close (fd3[0]);
  return 0;
}