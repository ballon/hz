#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define maxn 1002
#define maxy 1000

int main(int argc, char **argv)
{
	char xlocal[maxn+2][maxy];
    char xnew[maxn+2][maxy];
    char x[maxn+2][maxy];
    
    int        rank, value, size, i, j;
    int        i_first, i_last;
    MPI_Status status;
    
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if (size != 4) MPI_Abort( MPI_COMM_WORLD, 1 );

    int N, iterations;
    if(rank==0) {
    	if (argc != 5) { 
	        fprintf(stderr, "Usage: %s N input_file iterations output_file\n", argv[0]); 
	        return 1; 
	    } 
	    N = atoi(argv[1]); 
	    iterations = atoi(argv[3]); 
	    FILE* input = fopen(argv[2], "r"); 
	    for (int i = 0; i < N; ++i) { 
	        fscanf(input, "%s", x[i]); 
	    } 
	    fclose(input);
    }

    int on_each = maxy*maxy / size;
  
    MPI_Scatter(x[0], on_each, MPI_CHAR,
		 xlocal[1], on_each, MPI_CHAR,
		 0, MPI_COMM_WORLD );

    
    i_first = 1;
    i_last  = maxy/size;

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    while(iterations>0) {
    	iterations--;
		// Send up unless I'm at the top, then receive from below 
		if (rank > 0)
			MPI_Send( xlocal[1], maxy, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD );
		if (rank < size - 1)
			MPI_Recv( xlocal[maxy/size+1], maxy, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &status );

		// Send down unless I'm at the bottom 
		if (rank < size - 1)
			MPI_Send( xlocal[maxy/size], maxy, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD );
		if (rank > 0)
			MPI_Recv( xlocal[0], maxy, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status );

		// Send 0 row to end
		if(rank==0) MPI_Send(xlocal[1], maxy, MPI_CHAR, size-1, 2, MPI_COMM_WORLD);
		if(rank==size-1) MPI_Recv(xlocal[maxy/size+1], maxy, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);

		// Send last row to begin
		if(rank==size-1) MPI_Send(xlocal[maxy/size], maxy, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
		if(rank==0) MPI_Recv(xlocal[0], maxy, MPI_CHAR, size-1, 3, MPI_COMM_WORLD, &status);
		
		for (i=i_first; i<=i_last; i++)
			for (j=0; j<maxy; j++) {
				int alive = 0;
				for(int dx=-1; dx<=1; ++dx)
					for(int dy=-1; dy<=1; ++dy) {
						int fx=i+dx, fy=j+dy;
						if(fx==i && fy==j) continue;
						if(fy==-1) fy=N-1;
						if(fy==N) fy=0;
						alive += xlocal[fx][fy]=='X';
					}
				xnew[i][j]=xlocal[i][j];
				if(xlocal[i][j]=='X') {
					if(alive<2 || alive>3) xnew[i][j]='.';
				}else 
					if(alive==3) xnew[i][j]='X';
			}

		for (i=i_first; i<=i_last; i++)
			for (j=0; j<maxy; j++)
				xlocal[i][j] = xnew[i][j];
    }

    // Collect the data into x and print it 
    MPI_Gather( xlocal[1], maxy * (maxy/size), MPI_CHAR, x, maxy * (maxy/size), MPI_CHAR, 0, MPI_COMM_WORLD );
    if(rank==0) {
    	char buf[maxy];
	    FILE* fw = fopen(argv[4], "w");
		for (int i = 0; i < N; ++i) {
			memset(buf, 0, sizeof(buf));
	        strncpy(buf, x[i], maxy);
	        //buf[N] = 0;
	        fprintf(fw, "%s\n", buf);
	    }
	}
    MPI_Finalize( );
    
    return 0;
}
    

    