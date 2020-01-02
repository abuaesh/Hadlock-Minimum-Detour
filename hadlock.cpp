/*****************************
HADLOCK MINIMUM DETOUR MODEL
******************************
This program implements the famous Hadlock Detour algorithm,
based on the A* search.
******************************
By:
	Noha Abuaesh 	noha.abuaesh@gmail.com
	Walaa Elsadek	walaa.farouk@aucegypt.edu
*/
#include  <iostream>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <vector>


using namespace std;

enum Colors { blue=1, green, cyan, red, purple, yellow, grey, dgrey, hblue, hgreen, hred, hpurple, hyellow, hwhite };
//node data structure
struct node
{
	 int i;
	 int j;
	 int detour;
	 int d;
	 bool expanded;
	 node* next;
	 node* parent;
 };
//constants
const double BLOCKAGE_PERCENT = 0.4;
const char BLOCKED_PIN = 219;
const char EMPTY_PIN = ' ';
const char SRC_MARKER = 'S';	//marks source pin
const char TGT_MARKER = 'T';	//marks target pin
const char VISITED_PIN = 176;	//marks target pin
const char ROUTED_PIN = 178;	//marks a pin on the selected path between source and target pins
const char NEIGBHBOR_PIN = 145;
//global vars
double Lx, Ly, Lxy;				//dimension lengths in the grid
int ptr_lst=8;					//Detailed Output first item in the list to print

//function prototypes
void coutc(int color, char output);						//print colored character
void DrawBoard(int length, int width, vector<vector<char> >& grid, int prt_det); //DrawGrid
void set_grid(int length,int width,vector<vector<char> >& grid, double blockage_percent);
														//sets the grid: 
														//colored square character (219 ASCII code)for BLOCKED_PIN, 
														//empty char for EMPTY_PIN
void set_src_n_tgt(vector<vector<char> >& grid,int length, int width, int& srci, int& srcj, int& tgti, int& tgtj);
														//gets from the user i & j indices for source and target pins
bool validate_pin(int i, int j, bool print_err,vector<vector<char> >& grid,int length, int width);		
	//returns true if the pin located at grid location[i][j] is inside the grid boundaries and is an empty cell, 
	//returns false otherwise. print_err indicates if the user needs to be informed of the reason of invalidation in case the pin is invalid
int route(vector<vector<char> >& grid,int length, int width, int srci, int srcj, int tgti, int tgtj, int prt_det);
int calculate_detour(int i1, int j1, int i2, int j2, int srci, int srcj, int tgti, int tgtj);			
	//calculates the total detour for the path coming from the source node(i1,j1) to the target node passing through node2(indices i2&j2)
double calculate_distance(int i1, int j1, int i2, int j2);													//used by calculate_detour(); finds the distance between 2 given nodes
node* get_neighbors(node *n, node &P,vector<vector<char> >& grid,int length, int width,int srci, int srcj, int tgti, int tgtj);							
	//returns all the valid neighbors of the given node, sorted in ascending detours from current node
node* create_neighbor_node(int ni, int nj, node &P,vector<vector<char> >& grid,int length, int width,int srci, int srcj, int tgti, int tgtj);	
	//returns the neighbor node at index ni,nj if a valid node, NULL otherwise
void insert_node(node* &list, node &n);					
	//inserts the given node n into the list according to ascending order of detours. returns the new list. used by route()
void clean_memory(node* P);							
	//deletes all the dynamic nodes created
void clear_v_markers(vector<vector<char> >& grid,int length, int width,int srci, int srcj, int tgti, int tgtj);									//needed by route()
void remove_node(node * &, node &);

int main()
{
//   Console::WriteLine(L"Assignment 2");
Step1:									// Initialize partitions the current partitions
	system("CLS");
	coutc(hgreen, ' ');

	cout << "\n\nAssignment 2:";
	cout <<	"\n-------------" << endl;
	coutc(grey, ' ');


	int length, width; double blockage_percent; double blockp; int TF;
	int srci, srcj, tgti, tgtj;				//indices for source and target pins respectively
	do {  
		cout << "\nEnter Grid Length :  ";  
		cin >> length;
	}  while ( length<4);
	do {  
		cout << "\nEnter Grid Width  :  ";  
		cin >> width;
	}  while ( width<4);
	Lx=0;Ly=0;
	do {  
		cout << "\nEnter Cell Length [4] :  ";  
		cin >> Lx;
	}  while ( Lx<1);
	do {  
		cout << "\nEnter Cell Width  [3] :  ";  
		cin >> Ly;
	}  while ( Ly<1);
	do {  
		cout << "\nEnter Block % [0-100] :  ";  
		cin >> blockp;
	}  while ( blockp<0 || blockp>100);
	do {  
		cout << "\nPrint Detail [ Yes:1  No:0 ]:  ";  
		cin >> TF;
	}  while ( TF<0 || TF>1);
	if (TF)
	{ 
		do {  
			cout << "\nSize Print List [8]:  ";  
			cin >> ptr_lst;
		}  while ( ptr_lst<0 );
	}
	blockage_percent=blockp/100;
	Lxy=sqrt(pow(Lx,2)+pow(Ly,2));
	vector<vector<char> >  grid(width, vector<char>(length));	//the board is represented as a 2-D array grid
	set_grid(length,width,grid,blockage_percent);
	DrawBoard(length,width,grid,0);
	set_src_n_tgt(grid,length,width,srci,srcj,tgti,tgtj);
	if (TF) {DrawBoard(length,width,grid,TF); cout << "\n\n";  coutc(grey, ' '); system("pause");}
	int path=route(grid,length,width,srci,srcj,tgti,tgtj,TF);
	if (path){
		DrawBoard(length,width,grid,TF);
		cout << "\n\n";  coutc(grey, ' '); system("pause");}
	goto Step1;
	return 0;
}

void coutc(int color, char output)
{
	HANDLE handle= GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute( handle, color);
	cout<< output;
	SetConsoleTextAttribute( handle, color);
//	SetConsoleTextAttribute(handle, color |FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_INTENSITY);
}

void DrawBoard(int length, int width, vector<vector<char> >& grid, int prt_det)
{
	if (prt_det)	system("CLS");

	int ww,ll,cl,cw;
	if (width  >= 0   && width  <=10  )		ww=0;
	if (width  >= 11  && width  <=100 )		ww=1;
	if (width  >= 101 && width  <=1000)		ww=2;
	if (length >= 0   && length <=10  )		ll=0;
	if (length >= 11  && length <=100 )		ll=1;
	if (length >= 101 && length <=100 )		ll=2;
	
	int gl=3; int l=gl+1; int fx=l;
	int glength=fx+gl+length*l;  
	int gwidth=2+width*2;  
	
	printf(" \n");
	for(int x=0; x<glength; x++) coutc(yellow, '-');


	for(int y=0; y<gwidth; y++)
        for(int x=0; x<glength; x++)
			if(y==0)
                if(x==glength-1 || x==0)	printf(" \n");
                else if(x==1)				coutc(hgreen, ' '); //putchar(' ');
				else if(x%l==0 && x!=4)			
				{
					cl=(x-gl-fx)/l; printf("%d",cl);
					if (cl > 9  && cl <100)		x++;
					if (cl > 99 && cl <1000)	x+=2;
				} 
                else					putchar(' ');
            else if (y==1)
                if(x>=0 && x<=(fx+1))	putchar(' ');
                else if(x==(fx+2))		putchar(218);
                else if(x==glength-1)	printf("%c\n", 191);
                else if(x%l==2)			putchar(194);
                else					putchar(196);
            else if(y==gwidth-1)
                if(x>=0 && x<=(fx+1))	putchar(' ');
                else if(x==(fx+2))		putchar(192);
                else if(x==glength-1)	printf("%c\n", 217);
                else if(x%l==2)			putchar(193);
                else					putchar(196);
            else if(y%2==1)
                if(x>=0 && x<=(fx+1))	putchar(' ');
                else if(x==(fx+2))		putchar(195);
                else if(x==glength-1)	printf("%c\n", 180);
                else if(x%l==2)			putchar(197);
                else					putchar(196);
            else
				if(x==0)				putchar(' ');
				else if(x==1)
				{
					cw=(y-2)/2; printf("%d",cw);int n;
					if (cw >= 0   && cw <10)	n=fx;
					if (cw >= 10  && cw <100)	n=fx-1;
					if (cw >= 100 && cw <1000)	n=fx-2;
					for(int i=0; i<n; i++) {putchar(' ');x++;}
				}
                else if(x==glength-1)	printf("%c\n", 179);
                else if(x%l==2)			putchar(179);
                else if(x%l==3)			
				{
					for(int i=x; i<(x+gl); i++)
					{
						if(i%l==0)	{
							cl=(i-gl-fx)/l;	cw=(y-2)/2;
							if (grid[cw][cl]==BLOCKED_PIN)	
									coutc(hgreen, grid[cw][cl]); // <- Cell content goes here x=3+l*row no, y=2+2*col no
							else if (grid[cw][cl]==SRC_MARKER || grid[cw][cl]==TGT_MARKER)	
									coutc(hpurple, grid[cw][cl]); 
							else if (grid[cw][cl]==VISITED_PIN )	
									coutc(hyellow, grid[cw][cl]); 
							else if (grid[cw][cl]==NEIGBHBOR_PIN )	
									coutc(cyan, grid[cw][cl]); 
							else	coutc(hblue, grid[cw][cl]);}
						else	coutc(hgreen, ' '); 
					}
				}
	printf(" \n"); 
	for(int x=0; x<glength; x++) coutc(yellow, '-');
	printf(" \n");

}

void set_grid(int length,int width,vector<vector<char> >& grid, double blockage_percent)
{
	srand((unsigned) time (NULL));
	int island_cells;										//specifies the number of BLOCKED_PIN cells
	island_cells = (length*width) * blockage_percent;			//40% of te grid is BLOCKED_PIN
	
	for(int i=0; i < island_cells; i++)						// sets only the islands randomly
	{
		int i_index = rand() % width;
		int j_index = rand() % length;
		grid[i_index][j_index] = BLOCKED_PIN;				//put an BLOCKED_PIN cell 
	}
		
	for(int i = 0; i < width; i++)//sets the remaining unassigned cells to EMPTY_PIN
		for(int j = 0; j< length; j++)
		{
			if(grid[i][j] != BLOCKED_PIN )//checks if the current cell was not set to be an BLOCKED_PIN
				grid[i][j] = EMPTY_PIN; //set the current cell to be EMPTY_PIN cell
		}
}
void set_src_n_tgt(vector<vector<char> >& grid,int length, int width, int& srci, int& srcj, int& tgti, int& tgtj)
{
	coutc(grey, ' ');
	do{
		cout << endl << "Enter SOURCE i and j indices respectively: ";
		cin >> srci >> srcj;
	}while(!validate_pin(srci,srcj,true,grid,length,width));
	grid[srci][srcj] = SRC_MARKER;
	
	do{
		cout << endl << "Enter TARGET i and j indices respectively: ";
		cin >> tgti >> tgtj;
	}while(!validate_pin(tgti,tgtj,true,grid,length,width));
	grid[tgti][tgtj] = TGT_MARKER;
}

bool validate_pin(int i, int j, bool print_err,vector<vector<char> >& grid,int length, int width)
{
	bool valid_pin = true;
	char * err_msg = "Pin Validated\n";


	if(i > width-1 || i < 0 || j > length-1 || j < 0)
	{
		valid_pin = false;
		err_msg = "Pin Out of Bound.\n";
	}
	else if(grid[i][j]==BLOCKED_PIN || grid[i][j]==SRC_MARKER || grid[i][j]==VISITED_PIN)
	{
		valid_pin = false;
		err_msg = "Pin Blocked.\n";
	}

	if(print_err)
		cout << "Pin " << i << ":" << j << " - " << err_msg;

	return valid_pin;
}

int route(vector<vector<char> >& grid,int length, int width, int srci, int srcj, int tgti, int tgtj, int  prt_det)
{
	node S; //start from source node
	S.i=srci; S.j=srcj; S.detour=0; S.expanded=false; S.next=NULL; S.parent=NULL; S.d=0; 
	node* head = &S;	//always points to the head of the list
	node* ptr_neighbors=head;
	node* ptr = head; 
	node* visited_list=head;
	
	do{
		if(!((*ptr).expanded))
		{
			ptr_neighbors = get_neighbors(ptr_neighbors,*ptr,grid,length,width,srci,srcj,tgti,tgtj);
			(*ptr).expanded = true; //when you expand a node, mark it as expanded so that not to expand it again
			grid[(*ptr).i][(*ptr).j] = VISITED_PIN;
			if (prt_det) DrawBoard(length,width,grid,prt_det);
			node* pt = ptr_neighbors; 
			if (prt_det)
			{
				coutc(hyellow, ' ');
				cout << "\n** Node(" << (*ptr).i << "," << (*ptr).j << ")";
				if ((*ptr).parent)
					cout << " \t\t\tParent (" << (*(*ptr).parent).i << "," << (*(*ptr).parent).j << "), Detour="<< (*ptr).detour <<"\n"; 
				else cout << "\n";
				coutc(cyan, ' ');
				int ls=0;
				while(pt && ls<ptr_lst)
				{ 
//					if (!(((*pt).i==(*ptr).i) && ((*pt).j==(*ptr).j)) )
					if (pt!=ptr)
					{
						cout << "\nDetour Node(" << (*pt).i << "," << (*pt).j << ") = " << (*pt).detour << "    \tParent (" << (*(*pt).parent).i << "," << (*(*pt).parent).j << ") , d=" << (*pt).detour;	
						ls++;
					}
					pt = (*pt).next;
				}
				cout << "\n\n";  coutc(grey, ' '); system("pause");
			}

			node* tmp=ptr; ptr=ptr_neighbors;
			while(tmp == ptr) ptr=(*ptr).next;
//			if (!(((*tmp).i==srci) && ((*tmp).j==srcj)) )
			if (tmp!=head)			//SRC node is already added
			{
				visited_list->next=tmp;
				visited_list = (*visited_list).next;
			}
			remove_node(ptr_neighbors, *tmp);

		}
		else//node at ptr already expanded before
		{
			ptr = (*ptr).next;//keep searching the list for an unexpanded node
		}	

	}while(ptr && !(((*ptr).i==tgti)&&((*ptr).j==tgtj)));//loop until you find target node or until you can't find a node to visit
//	if(!((*ptr).i==tgti)&&((*ptr).j==tgtj))//if no path found
	if(!ptr)//if no path found
	{
		grid[srci][srcj] = SRC_MARKER;
		DrawBoard(length,width,grid,prt_det);
		coutc(hpurple, ' ');
		cout << "\n\nCANNOT FIND A PATH FROM SOURCE NODE TO TARGET NODE..." << endl << endl;
		cout << "\n\n";  coutc(grey, ' '); system("pause");
		return 0;

	}
	else //a path found, ptr currently pointing to target pin
	{
		while(visited_list->parent)//mark the path if one exists
		{
			grid[(*visited_list).i][(*visited_list).j] = ROUTED_PIN;
			visited_list = (*visited_list).parent;
			if (prt_det)	DrawBoard(length,width,grid,prt_det);
		}
	}
	
	if (head)	clean_memory(head);
	if (ptr)	clean_memory(ptr);
	clear_v_markers(grid,length,width,srci,srcj,tgti,tgtj);
	return 1;
}
int calculate_detour(int i1, int j1, int i2, int j2, int srci, int srcj, int tgti, int tgtj) //detour in the path from node1(i1,j1)to the target node tgt when passing through node2(i2,j2)
 {
	 int detour = 0;
	 detour = calculate_distance(i1,j1,i2,j2) + calculate_distance(i2,j2,tgti,tgtj) - calculate_distance(i1,j1,tgti,tgtj); 
	 return detour;
 }

double calculate_distance(int i1, int j1, int i2, int j2)
 {
	 double distance = 0;
	 int delta_i = abs(i1-i2);
	 int delta_j = abs(j1-j2);

	 if( delta_i >= delta_j )
		 distance = Lx*delta_i + (Lxy-Lx)*delta_j;
	 else //if(abs(i1-i2) < abs(j1-j2))
		 distance = Ly*delta_j + (Lxy-Ly)*delta_i;
	 return distance;
 }

void clear_v_markers(vector<vector<char> >& grid,int length, int width, int srci, int srcj, int tgti, int tgtj)
 {
	 for(int i=0; i<width; i++)
		 for(int j=0; j<length; j++)
		 {
			 if(i==srci && j==srcj)
				 grid[i][j] = SRC_MARKER;
			 else
				 if(i==tgti && j==tgtj)
					 grid[i][j] = TGT_MARKER;
				 else
					 if(grid[i][j]==VISITED_PIN || grid[i][j]==NEIGBHBOR_PIN)
						 grid[i][j] = EMPTY_PIN;
		 }
 }

node* get_neighbors(node *n, node &P,vector<vector<char> >& grid,int length, int width,int srci, int srcj, int tgti, int tgtj) //returns all the valid neighbors of the given node
 {
	//the list of neighbors will be inserted here one by one according to ascending detours
	int i = P.i, j = P.j;
	int ni, nj;				//indices for neighbor nodes
	node* ptr = NULL;		//pointer that will point at the neighbor node

	//check all the 8 neighbors of node i,j
	//neighbor 1: 
	ni = i-1;	 nj = j-1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through					
		insert_node(n,*ptr); 					
	//neighbor 2: 
	ni = i-1;	 nj = j;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through
		insert_node(n,*ptr); 					
	//neighbor 3: 
	ni = i-1;	 nj = j+1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through				
		insert_node(n,*ptr); 					
	//neighbor 4: 
	ni = i;	 nj = j-1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through					
		insert_node(n,*ptr); 					
	//neighbor 5: 
	ni = i;	 nj = j+1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through
		insert_node(n,*ptr); 					
	//neighbor 6: 
	ni = i+1;	 nj = j-1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through
		insert_node(n,*ptr); 					
	//neighbor 7: 
	ni = i+1;	 nj = j;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through
		insert_node(n,*ptr); 					
	//neighbor 8: 
	ni = i+1;	 nj = j+1;
	if(ptr= (create_neighbor_node(ni,nj,P,grid,length,width,srci,srcj,tgti,tgtj)))//pin is valid to route through
		insert_node(n,*ptr); 					

	return n;
 }

node* create_neighbor_node(int ni, int nj, node &P,vector<vector<char> >& grid, int length, int width,int srci, int srcj, int tgti, int tgtj)
 {
	 node* ptr = NULL;
	 int i = P.i, j = P.j;
	 if(validate_pin(ni,nj,false,grid,length,width))//pin is valid to route through
	 {
		 ptr = new node();
		(*ptr).i = ni;
		(*ptr).j = nj;
		(*ptr).d = calculate_detour(i,j,ni,nj,srci,srcj,tgti,tgtj);
		(*ptr).detour = (*ptr).d + P.detour;
		(*ptr).expanded = false;
		grid[ni][nj] = NEIGBHBOR_PIN;
		(*ptr).next = NULL;
		(*ptr).parent = &P;
	 }
	return ptr;	 
 }

void insert_node(node* &list, node &n) 
//inserts the given node n into the list according to ascending order of detours, list always points to the head of the list
 {
	node* temp = &n; 
	node* temp2 = list; 
	node** temp3 = &list; 
	if (temp != NULL && temp2 != NULL ){
		while(temp2 != NULL && temp2->detour < temp->detour) 
		{ 
			temp3 = &temp2->next; 
			temp2 = temp2->next; 
		} 	
	}		
	*temp3 = temp; 
	temp->next = temp2;
 }

void remove_node(node* &list, node &n)
 {
	node* ptr=list; node* prev=NULL;
	if (ptr == &n && !(ptr -> next))  list=NULL;
	else if (ptr == &n && (ptr -> next))  list=n.next;
	else
	{
		while(ptr && (ptr != &n))
		{
			prev = ptr;
			ptr = ptr -> next;
		}
		if (ptr == &n && prev)  prev->next = n.next;
	}
 }

void clean_memory(node* P)//deletes all the dynamic nodes linked to P(next or parent)
 {
	 node* ptr = P; 
//	 node* temp2 = ptr;
	 while(ptr->next)
	 {
		node* temp1 = ptr;
		temp1=ptr;
		ptr=ptr->next; 
		temp1=NULL;
/*		temp2 = ptr->parent;
		ptr = (*ptr).next;
		delete(temp1);
		delete(temp2);
		temp1 = ptr;
		 */
	 }
	 if (ptr)	ptr=NULL;
 }


