from mp1_heap import *
import copy

# In[]

class AstarNode:
    i = 0;
    j = 0;
    h = 0;
    g = 0;
    
    def  __init__(self,i,j,h,g):
        self.i = i;
        self.j = j;
        self.h = h;
        self.g = g;

    def __lt__(self,other):
        return (self.h+self.g) < (other.h+other.g)

def makeAstarNode(i,j,h,g):
    node = AstarNode(i,j,h,g);
    return node;

# In[]


def myAstar(maze,start,end):
    height = len(maze);
    width  = len(maze[0]);
    print("ASTAR Maze has size height:%s by width:%s" %(height,width));
    
    maze_sol = copy.deepcopy(maze);   
    node_count = 0;
    
    
    endi, endj =   end[0],  end[1];   
    i,j        = start[0],start[1];    

    # heuristic
    h = abs(endi-i)+abs(endj-j);
    
    # frontier - priority queue
    q = [];
    
    # trace matrix for bookkeeping
    # each entry = [previ, prevj, cost]
    prevtrace = [[-1,-1,-1] for idx in range(height*width)] 
    prevtrace[i*width+j] = [i,j,h];
  
    s = makeAstarNode(i,j,h,0)   
    heappush(q,s);   
   
    # while frontier is not empty
    while(len(q) != 0):

        curr = heappop(q);
        node_count = node_count + 1;
        i = curr.i;
        j = curr.j
        # debug
        #print("Now at (%s,%s), having \"%s\"" %(i,j,maze[i][j]))

        # case found
        if(maze[i][j] == '.'):
            # solution is found
            print("ASTAR:Found target \"%s\" at (%s,%s)!" % (maze[i][j],i,j));
            break;
        
        # iterative cases
        # if can travel up    --- 1
        if(maze[i-1][j] != '%'):
            h = abs(endi-(i-1))+abs(endj-j);
            g = curr.g+1;
            if (prevtrace[(i-1)*width+j][2] > h+g or prevtrace[(i-1)*width+j][2] ==-1):
                heappush(q,makeAstarNode(i-1,j,h,g));
                prevtrace[(i-1)*width+j] = [i, j, h + g];

        # if can travel right  --- 2
        if(maze[i][j+1] != '%'):
            h = abs(endi-(i))+abs(endj-(j+1));
            g = curr.g+1;
            if (prevtrace[i*width+(j+1)][2] > h+g or prevtrace[i*width+(j+1)][2] ==-1):
                heappush(q,makeAstarNode(i,j+1,h,g));
                prevtrace[i*width+(j+1)] = [i, j, h + g];

        # if can travel down   --- 3
        if(maze[i+1][j] != '%'):
            h = abs(endi-(i+1))+abs(endj-j);
            g = curr.g+1;
            if (prevtrace[(i+1)*width+j][2] > h+g or prevtrace[(i+1)*width+j][2] ==-1):
                heappush(q,makeAstarNode(i+1,j,h,g));
                prevtrace[(i+1)*width+j] = [i, j, h + g];

        # if can travel left   --- 4
        if(maze[i][j-1] != '%'):
            h = abs(endi-(i))+abs(endj-(j-1));
            g = curr.g+1;
            if (prevtrace[i*width+(j-1)][2] > h+g or prevtrace[i*width+(j-1)][2] == -1):
                heappush(q,makeAstarNode(i,j-1,h,g));
                prevtrace[i*width+(j-1)] = [i, j, h + g];
    
   
    # end of while loop
    # verify
    if(maze[i][j] != '.'):
        print("Error, no target has been found!");
        return;
    
    if((i,j)!= (endi, endj)):
        #print("Error, found target not match with real target!");
        return {"indicator":-1,"cost":prevtrace[i*width+j][2], "n_node":node_count,"target":(i,j)};
    
    # back trace
    y,x = endi,endj;
    k = node_count;
    while((y,x) != (start[0],start[1])):
        if (k == 0):
            break;
        k = k-1;
        maze_sol[y][x]='.';
        y = prevtrace[y*width+x][0];
        x = prevtrace[y*width+x][1];
        
    
    
    result = {"maze_sol":maze_sol, "cost":prevtrace[i*width+j][2], "n_node":node_count};
    return result;

