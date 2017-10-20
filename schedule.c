#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAXLINE 100

/*global variables*/
struct tasks
{
	char name[MAXLINE];
	unsigned long time;
}p;

int task_num = 0;

/*Functions*/
void clear();
void tasknum(FILE *fd);
void start(FILE *fd , FILE *out);
void view_tasks(FILE *fd);
void add_task(FILE *fd,FILE *out);
void delete_task(FILE *fd , FILE *out);
void weekly_reset(FILE *fd, FILE *out);
void file_exist();
//void clear();
int make_temp(char *template);

int main () 
{
	file_exist();		/*check file exists*/
	
	FILE *fd , *out;
	weekly_reset(fd,out);	/*resetting the times to 0 after every week*/
	char buf[MAXLINE];
	tasknum(fd);		

	/*title screen*/
	putchar('\n');
	printf("\t\t\t\t--------------------------\n");
	printf("\t\t\t\t|-----Scheduler program-----|\n");
	printf("\t\t\t\t---------------------------\n");
	putchar('\n');

	/*shell*/
	printf("%% ");
	while(fgets(buf,MAXLINE,stdin)!= NULL)
	{
		tasknum(fd);

		if(buf[strlen(buf)-1 == '\n'])
			buf[strlen(buf)-1] = 0;

		if(strcmp(buf,"start")==0)
		{
			start(fd,out);
		}
		else if (strcmp(buf,"tasks")==0)
		{
			view_tasks(fd);
		}
		else if (strcmp(buf,"add")==0)
		{
			add_task(fd,out);
		}
		else if (strcmp(buf,"delete")==0)
		{
			delete_task(fd,out);
		}
		else if (strcmp(buf,"exit")==0)
		{
			exit(0);
		}
		else if (strcmp(buf,"clear")==0)
		{
			clear();
		}
		printf("%% ");
	}

	return 0;
}
void file_exist()
{
	FILE *fd;
	struct stat sbuf;

	if(stat("structure.txt",&sbuf) < 0)
	{
		/*file doesnt exist*/
		fd = fopen("structure.txt","w+");
		fclose(fd);	
	}
}

/*will reset the times of tasks each week*/
void weekly_reset(FILE *fd , FILE *out)
{
	char buffer[32];
	struct tm *ts;
	size_t last;
	time_t timestamp = time(NULL);

	ts = localtime(&timestamp);
	last = strftime(buffer,32,"%A",ts);
	buffer[last] = '\0';

	if(strcmp(buffer,"Monday")==0)
	{
		fd = fopen("structure.txt","r+");
		char good_template[] = "/tmp/dirXXXXXX";
		/*open temp file*/
		int ff2 = make_temp(good_template);
		/*use standard io*/
		out = fdopen(ff2,"r+");

		/*reading resetting time for each and writing to temp and then to structure.txt*/
		while(fread(&p,sizeof(struct tasks),1,fd)!=0)
		{
			p.time = 0;
			if(!fwrite(&p,sizeof(struct tasks),1,out))
				perror("fwrite");
		}
		rewind(out);
		rewind(fd);

		while(fread(&p,sizeof(struct tasks),1,out))
		{
			if(!fwrite(&p,sizeof(struct tasks),1,fd))
				perror("fwrite");
		}
		//close
		fclose(fd);
		fclose(out);
		close(ff2);
		unlink(good_template);
	}

	tasknum(fd);
	
}
/*delete task from the file*/
void delete_task(FILE *fd, FILE *out)
{
	//printf("________________________________________________________________\n");
	char buf[MAXLINE];
	printf("\tName: ");
	if(!fgets(buf,MAXLINE,stdin))
		perror("fgets");
	if(buf[strlen(buf)-1]=='\n')
		buf[strlen(buf)-1] = 0;

	
	fd = fopen("structure.txt","r+");
	/*create a temp file*/
	int ffd;
	char good_template[] = "/tmp/dirXXXXXX";
	ffd = make_temp(good_template);
	out = fdopen(ffd,"r+");

	/*now writing everything to temp execpt name to be deleted*/
	while(fread(&p,sizeof(struct tasks),1,fd)!=0)
	{
		if(strcmp(buf,p.name)!=0)
		{
			if(!fwrite(&p,sizeof(struct tasks),1,out))
				perror("fwrite");		}
	}
	fclose(fd);
	fd = fopen("structure.txt","w+");
	rewind(out);

	while(fread(&p,sizeof(struct tasks),1,out)!=0)
	{
		if(!fwrite(&p,sizeof(struct tasks),1,fd))
			perror("fwrite");
	}
	
	/*closing file descriptors and unlinking temp file*/
	fclose(fd);
	fclose(out);
	close(ffd);
	unlink(good_template);



}
/*adding task to list*/
void add_task(FILE *fd , FILE *out)
{
//	printf("____________________________________\n");
	char buf[MAXLINE];
	printf("\tName: ");
	if(!fgets(buf,MAXLINE,stdin))
		perror("fgets");

	if(buf[strlen(buf)-1]=='\n')
		buf[strlen(buf)-1] = 0;

	strcpy(p.name,buf);
	p.time = 1;

	/*append to structure.txt*/
	fd = fopen("structure.txt","a");
	if(!fwrite(&p,sizeof(struct tasks),1,fd))
		perror("fwrite");

	fclose(fd);
	tasknum(out);
}
/*counting the number of tasks, this is for the array declared in start(HOWEVER hanging issues need fixing)*/
void tasknum(FILE *fd)
{
	task_num = 0;
	fd = fopen("structure.txt", "r");

	while(fread(&p,sizeof(struct tasks),1,fd)!=0)
	{
		task_num++;
	}
	fclose(fd);
}
/*read tasks from file and output them to screen*/
void view_tasks(FILE *fd)
{
	/*change so we show it in h,m,s*/

	int i = 1;
	long hr , t , min , sec;
	hr=t=min=sec=0;

	fd = fopen("structure.txt","r");
	printf("\t-----------------------------\n");
	while(fread(&p,sizeof(struct tasks),1,fd) != 0)
	{
		hr = p.time/3600;
		t = p.time%3600;
		min = t/60;
		sec = t%60;

		printf("\t|%d-%s & %ldh.%ldm.%lds|\n",i++,p.name,hr,min,sec);
		hr=t=min=sec=0;
	}
	printf("\t----------------------------\n");
	putchar('\n');
	fclose(fd);
}
/*create a temporary file*/
int make_temp(char *template)
{
	int fd;
	struct stat sbuf;

	if((fd = mkstemp(template)) < 0)
		perror("cant create temp file");

	//printf("temp name = %s\n" , template);

	if(stat(template,&sbuf) < 0)
	{
		if(errno == ENOENT)
			printf("file doesnt exist\n");
		else
			perror("stat failed");
	}
	else
	{
	//	printf("file exists\n");
		return fd;
	}
}
/*starting scheduler*/
void start(FILE *fd , FILE *out)
{
	int flag = 0;
	/*variables*/
	time_t timer;
	struct tm y2k = {0};
	unsigned long seconds , seconds_now;
	char start , finish;
	int times[task_num];
	times[task_num] = '\0';
	int num,i,read;
	num=i=read=0;

	int ff2;
	

	y2k.tm_hour = 0; y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year=100; y2k.tm_mon = 0; y2k.tm_mday=1;

	/*opening file to read the tasks for today*/

	fd = fopen("structure.txt","r+");
	if(!fd)
		perror("fopen");

	FILE *number;
	tasknum(number);

	/*creating a temporary file*/
	char good_template[] = "/tmp/dirXXXXXX";
	ff2 = make_temp(good_template);

	/*using standard IO for fd*/
	out = fdopen(ff2,"r+");

	printf("_________________________________________________________________________\n");

	/*begin processing daily tasks...*/
	while(fread(&p,sizeof(struct tasks),1,fd)!=0)
	{
		putchar('\n');
	//	printf("---------------------\n");
		printf("start: %s?",p.name);
		scanf(" %c" , &start);
		
		if(start == 'n')
		{
		//	p.time += 1;
			if(!fwrite(&p,sizeof(struct tasks),1,out))
				perror("fwrite");
		}
		else if (start == 'y')
		{
			time(&timer);
			seconds = difftime(timer,mktime(&y2k));

			printf("finish %s?:",p.name);
			scanf(" %c",&finish);

			if(finish=='y')
			{
				time(&timer);
				seconds_now = difftime(timer,mktime(&y2k));
				p.time += (seconds_now - seconds);

				if(!fwrite(&p,sizeof(struct tasks),1,out))
					perror("fwrite");
			}
		}
		
		
	}
//	printf("______________________________________________________________________\n");
	putchar('\n');
	/*now ordering tasks from temp file*/
	rewind(fd);
	rewind(out);

	while(fread(&p,sizeof(struct tasks),1,out)!=0)
	{
		times[num] = p.time;
		++num;
	}

	/*sorting times simple algorithm*/
	if(task_num > 1)
	{
		for(int j=1; j<task_num; j++)
		{
			int key = times[j];
			int i = j -1;

			while((i >= 0) && times[i] > key)
			{
				times[i+1] = times[i];
				i -=1;
			}
			times[i+1] = key;
		}
	}

	/*my own inefficient brute force algorithm for matching and ordering*/
	do
	{
		rewind(out);
		while(fread(&p,sizeof(struct tasks),1,out)!= 0)
		{
			if(p.time == times[i])
			{
				i++;
				if(!fwrite(&p,sizeof(struct tasks),1,fd))
					perror("fwrite");
				read++;
			}
		}
	}while(read != task_num);

	/*now we have overwritten structure.txt with the ordered times*/
	/*unlink temp file and close FDs*/
	fclose(fd);
	fclose(out);
	close(ff2);
	unlink(good_template);

}
void clear()
{
	pid_t pid;
	int status;
	char file[MAXLINE];
	
	strcpy(file,"./clear.sh\0");

	if((pid = fork())<0)
		perror("fork");
	else if (pid == 0)
	{
		execlp(file,file,(char*)0);
		perror("couldn't execute");
	}

	/*parent*/
	if((pid = waitpid(pid,&status,0))<0)
		perror("waitpid error");


	
		/*title screen*/
		putchar('\n');
		printf("\t\t\t\t--------------------------\n");
		printf("\t\t\t\t|-----Scheduler program-----|\n");
		printf("\t\t\t\t---------------------------\n");
		putchar('\n');

}
