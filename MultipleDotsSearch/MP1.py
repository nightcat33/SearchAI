
# coding: utf-8

# In[3]:


#debug
import time
import copy

from utility import *
import numpy as np
# from mp1_BFS import myBFS
# from mp1_DFS import myDFS
# from mp1_GRD import myGRD
from mp1_ASS import myAstar
from mp1_heap import *
from numpy import floor,mod




# In[4]:
def myBFS_for12(maze,start,dist_matrix,targets,start_idx,n_targets):
    
    height = len(maze);
    width  = len(maze[0]);

    maze_sol = copy.deepcopy(maze);    
    #print("Maze has size height:%s by width:%s" %(height,width));
    
    i,j = start[0],start[1];    
    maze_sol[i][j] = 'x';
    # trace matrix for bookkeeping
    trace = [[-1,-1] for idx in range(height*width)] 
    q = [];    
    q.append([i,j]);   
    #print(trace)
    
    # mark start point as [rank=0, past_direction = undefined]
    #           rank = number of steps from start to here
    # past_direction = direction taken to get here (1,2,3,4 are up,right,down,left)
    trace[i*width+j] = [0,0];
    
    dot_count = 0;
    # start while loop
    while(len(q) != 0):
        curr = q.pop(0);
        i = curr[0];
        j = curr[1];
        rank = trace[i*width+j][0];
        # print("Now at (%s,%s), having \"%s\"" %(i,j,maze[i][j]))        
        if(maze[i][j] == '.'):
            # solution is found
            #print("BFS: Found target \"%s\" at (%s,%s)!" % (maze[i][j],i,j));
            dot_count = dot_count+1;
            idx = targets.index((i,j));
            #print("start_idx= %s, idx = %s" % (start_idx,idx))
            dist_matrix[start_idx][idx] = trace[i*width+j][0];
            #print(dist_matrix)
            maze_sol[i][j] = 'x';
            #print("\n\n")
            #Path_Label = {'1' : }
            if(dot_count == n_targets):
                break;
        
        # if can travel up    --- 1
        if((maze[i-1][j] != '%') and (trace[(i-1)*width+j][0] == -1)):
            q.append([i-1,j]);
            trace[(i-1)*width+j] = [rank+1,1];
            #print(",pushed \"%s\"" % maze[i-1][j]);
        
        # if can travel right --- 2    
        if((maze[i][j+1] != '%') and (trace[i*width+j+1][0] == -1)):
            q.append([i,j+1]);
            trace[i*width+j+1] = [rank+1,2];   
            #print(",pushed \"%s\"" % maze[i][j+1]);
            
        # if can travel down  --- 3 
        if((maze[i+1][j] != '%') and (trace[(i+1)*width+j][0] == -1)):
            q.append([i+1,j]);
            trace[(i+1)*width+j] = [rank+1,3];            
            #print(",pushed \"%s\"" % maze[i+1][j]);
            
        # if can travel left --- 4    
        if((maze[i][j-1] != '%') and (trace[i*width+j-1][0] == -1)):
            q.append([i,j-1]);
            trace[i*width+j-1] = [rank+1,4];   
            #print(",pushed \"%s\"" % maze[i][j-1]);
        
    # return
    print("Found %s dots!" %(dot_count))
    return;
# In[part1.1]
# I/O
file_name = 'mediumMaze.txt';
#file_name = 'openMaze.txt';
#file_name = 'bigMaze.txt';

with open(file_name, 'r') as myfile:
    data = myfile.read()    
    
maze = [[x for x in line] for line in data.split('\n')];

# Initialize
start = [(index, row.index('P')) for index, row in enumerate(maze) if 'P' in row]
end = [(index, row.index('.')) for index, row in enumerate(maze) if '.' in row];
start_time = time.clock();

# part 1.1 Solve Maze
#result = myDFS(maze,start[0])
#result = myBFS(maze,start[0])
result = myAstar(maze,start[0],end[0])
#result = myGRD(maze)

# Display result
maze_sol = result["maze_sol"];
display(maze_sol);
print("Cost is %s, reached %s nodes." %(result["cost"],result["n_node"]));
running_time = time.clock()-start_time;
if(running_time > 1):
    print("Run time is %s sec" % (running_time))
else:
    print("Run time is %s msec" % (running_time*1000))


# In[5]:

idx_table = ['1','2','3','4','5','6','7','8','9','a','b','c','d',
             'e','f','g','h','i','j','k','l','m','n','o','p','q',
             'r','s','t','u','v','w','x','y','z','A',
             'B','C','D','E','F','G','H','I','J','K','L','M',             
             'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'];

class state:
    pp = 0;  # position x*width + height
    bv = (); # bit vector 
    
    def  __init__(self,pos,bv):
        self.pp = pos;
        self.bv = bv;
    def __hash__(self):
        return hash((self.pp, self.bv))

    def __eq__(self, other):
        return (self.pp, self.bv) == (other.pp,other.bv)

def newState(pos,bv):
    node = state(pos,bv);
    return node;


def eatAll_BFS(maze_in,start):
    
    maze = copy.deepcopy(maze_in);  
    maze[start[0]][start[1]] = ' '
    height = len(maze);
    width  = len(maze[0]);
    print(">> Multi-BFS: Maze has size height:%s by width:%s\n" %(height,width));
    
    ##################### target init ########################
    targets = [];
    for j in range(height):
        col_n = [i for i,x in enumerate(maze[j]) if x =='.']
        temp_idx = [0]*len(col_n)
        for k in range(len(col_n)):
            targets.append((j,col_n[k]))
    
    print(">> Targets:\n%s\n" % targets)
    
    ################# BFS implementation #####################
    q = []; #BFS Queue - stores states
    mymap = {};
    trace = {};
        
    s0 = newState(start[0]*width+start[1],tuple([1]*len(targets)));
    step = 0;    
    print(mymap.get(s0))
    mymap[s0] = step;
    trace[s0] = s0;
    print(mymap.get(s0))
    #print(s0.bv)
    
    q.append(s0);
    
    i,j = 0,0;
    cnt = 0;
    itr = 0;
    print(">> Begin:")
    ####################### BFS Loop #########################
    
    while(len(q) !=0):
        
        itr = itr+1;
        curr = q.pop(0);    
        i = int(curr.pp/width);
        j = mod(curr.pp,width)
        step = mymap[curr];
        previ,prevj = int(trace[curr].pp/width),mod(trace[curr].pp,width);
        
       
        
        #print("At(%s,%s), from(%s,%s) step = %s, %s " % (i,j,previ,prevj,step,(curr.bv)))
        
        if(sum(curr.bv) == 0):
            retval = mymap[curr];
            print("%s nodes expanded" % itr);
            ii = len(targets)-1;
            while((i,j) != start):
                if(maze[i][j]=='.'):
                    maze[i][j]=idx_table[ii];
                    ii = ii -1;
#                else:
#                    maze[i][j] = '*';
                curr = trace[curr];
                i,j = int(curr.pp/width),mod(curr.pp,width);
            display(maze)
            return retval;# Solution is found
        
        # Go UP
        iu,ju = i-1,j;
        if((maze[iu][ju] != '%')):   
            temp = list(curr.bv);
            if(maze[iu][ju] == '.'):
                cnt = cnt + 1;
                idx = targets.index((iu,ju));
                #display(maze)             
                temp[idx] = 0;
            su = newState(iu*width+ju,tuple(temp))
            if(mymap.get(su) == None):
                #print("pushing %s,%s,%s" % (iu,ju,su.bv))
                mymap[su] = step+1;
                trace[su] = curr;
                q.append(su);
        
         # Go RIGHT
        ir,jr = i,j+1;
        if((maze[ir][jr] != '%')):          
            temp = list(curr.bv);
            if(maze[ir][jr] == '.'):
                cnt = cnt + 1;
                idx = targets.index((ir,jr));
                #display(maze)             
                temp[idx] = 0;
            sr = newState(ir*width+jr,tuple(temp))
            if(mymap.get(sr) == None):
                #print("pushing %s,%s,%s" % (ir,jr,sr.bv))
                mymap[sr] = step+1;
                trace[sr] = curr;
                q.append(sr);
        
         # Go DOWN
        idd,jd = i+1,j;
        if((maze[idd][jd] != '%')):          
            temp = list(curr.bv);
            if(maze[idd][jd] == '.'):
                cnt = cnt + 1;
                idx = targets.index((idd,jd));
                #display(maze)             
                temp[idx] = 0;
            sd = newState(idd*width+jd,tuple(temp))
            if(mymap.get(sd) == None):
                #print("pushing %s,%s,%s" % (idd,jd,sd.bv))
                mymap[sd] = step+1;  
                trace[sd] = curr;
                q.append(sd);
                   
         # Go LEFT
        il,jl = i,j-1;
        if((maze[il][jl] != '%')):          
            temp = list(curr.bv);
            if(maze[il][jl] == '.'):
                cnt = cnt + 1;
                idx = targets.index((il,jl));
                #display(maze)             
                temp[idx] = 0;
            sl = newState(il*width+jl,tuple(temp))
            if(mymap.get(sl) == None):
                #print("pushing %s,%s,%s" % (il,jl,sl.bv))
                mymap[sl] = step+1;  
                trace[sl] = curr;
                q.append(sl);
    
        
    


# In[10]:

# this object type is what is pushed onto queue for A star search
class Astarstate:
    pp = 0; # pacman_position, expressed as pp=i*width+j
    bv = ();# bit_vector, a tuple such as (0,1,1,1,0,1,0,1...) (length=num_targets) where 0 is a eaten dot, 1 is uneaten
            # must use tuple (not list!) because list is no hashable (hence cannot use map!) in python
    h = 0;  # heuristic function, 
    g = 0;  # cost bookkeeping
    
    def  __init__(self,pos,bv,h,g):
        self.pp = pos;
        self.bv = bv;
        self.h = h;
        self.g = g;
        
    def __hash__(self):      # must provide this function, otherwise map won't work
        return hash((self.pp, self.bv))

    def __eq__(self, other): # must provide this function, otherwise map won't work
        return (self.pp, self.bv) == (other.pp,other.bv)
    
    def __lt__(self, other): # must provide this function, otherwise priority queue won't work
        return (self.h+self.g) < (other.h+other.g)
    
    def __bt__(self, other): # must provide this function, otherwise priority queue won't work
        return (self.h+self.g) > (other.h+other.g)
    
def newAstarstate(pos,bv,h,g):# not the constructor! just a regular function
    node = Astarstate(pos,bv,h,g);
    return node;

parent = dict()   # for MST
rank = dict()     # for MST

def make_set(vertice):
    parent[vertice] = vertice
    rank[vertice] = 0

def find(vertice):
    if parent[vertice] != vertice:
        parent[vertice] = find(parent[vertice])
    return parent[vertice]

def union(vertice1, vertice2):
    root1 = find(vertice1)
    root2 = find(vertice2)
    if root1 != root2:
        if rank[root1] > rank[root2]:
            parent[root2] = root1
        else:
            parent[root1] = root2
            if rank[root1] == rank[root2]: rank[root2] += 1

# heuristic generation based on MST formed by current position and all uneaten dots
def h_function(dist_matrix,maze,curr,targets,n_targets,huge_matrix):
    
    start_time = time.clock();
    temp = np.zeros((n_targets+1,n_targets+1)) # temp is a copy of dist_matrix, 
                                               # but it also has distance from 
                                               # pacman current position to all dots
    for i in range(n_targets):
        for j in range(n_targets):
             temp[i][j] = copy.deepcopy(dist_matrix[i][j]);
        temp[n_targets][i] = copy.deepcopy(huge_matrix[curr.pp][i]);
        temp[i][n_targets] = copy.deepcopy(huge_matrix[curr.pp][i]);
    
    check = list(curr.bv) # check is the bit-vector in curr (curr is an Astarstate defined above)
    
    edge_list = []; 
    
    for i in range(len(check)): 
        if(check[i] == 0):
            for j in range(n_targets+1):
                temp[j][i] = 0; # MODIFICATION:2017.09.29 remove eaten dots from temp, reduce nodes dramatically
                temp[i][j] = 0;
    
    for i in range(n_targets+1):
        for j in range(n_targets):
            if(temp[i][j] != 0):
                edge_list.append((i,j,temp[i][j]));
   
    edge_list.sort(key=lambda tup: tup[2]);
    
    
    ########### kruscal here ############    
    for v in range(n_targets+1):
        make_set(v)
        
    ret_val = 0;
    count = 0;
    for edge in edge_list:
        if(count == sum(check)):
            break;
        v1, v2, weight = edge
        if find(v1) != find(v2):
            count = count + 1;
            union(v1, v2)
            ret_val = ret_val + weight;
    #print("h = %s" % ret_val)
    #print("h_function takes %s msec" % (1000*(time.clock()-start_time)))
    return ret_val; # 100% admissible



def eatAll_Astar(maze_in,start):
    
    maze = copy.deepcopy(maze_in);  
    maze[start[0]][start[1]] = ' '
    height = len(maze);
    width  = len(maze[0]);
    print(">> Multi-Astar: Maze has size height:%s by width:%s\n" %(height,width));
    
    ##################### target init ########################
    targets = [];
    for j in range(height):
        col_n = [i for i,x in enumerate(maze[j]) if x =='.']
        temp_idx = [0]*len(col_n)
        for k in range(len(col_n)):
            targets.append((j,col_n[k]))
            
    n_targets = len(targets);
    print(">> Targets:\n%s\n" % targets)
    print("There are total of %s dots!" %n_targets);
    
    remaining_targets = copy.deepcopy(targets);
    ################## distance matrix #######################
    start_time = time.clock();
    dist_matrix = np.zeros(shape=(n_targets+1,n_targets))        
    
    for i in range(n_targets):
        start_k = targets[i];
        start_idx = targets.index(start_k);
        myBFS_for12(maze,start_k,dist_matrix,targets,start_idx,n_targets);
        
    myBFS_for12(maze,start,dist_matrix,targets,n_targets,n_targets);
    print("final matrix")
    print(dist_matrix)
    print("Dist matrix takes %s msec to build" % (1000*(time.clock()-start_time)) )
    
    ##################### huge matrix #######################
    start_time = time.clock();   
    huge_matrix = np.zeros(shape=(height*width,n_targets))        
    
    for i in range(height):
        for j in range(width):
            start_idx = i*width+j;
            if(maze[i][j] != "%"):
                start_k = (i,j);
                myBFS_for12(maze,start_k,huge_matrix,targets,start_idx,n_targets);
            #print("%s %s %s %s " % (i,j,maze[i][j],huge_matrix[start_idx]))
        
    print("final matrix")
    print(huge_matrix)
    print("Huge matrix takes %s msec to build" % (1000*(time.clock()-start_time)) )    
    
    
    
    ################## A* implementation #####################
    q = []; #BFS Queue - stores states
    qq = [];
    mymap = {};
    trace = {};#
        
    s0 = newAstarstate(start[0]*width+start[1],tuple([1]*len(targets)),0,0);
    step = 0;    
    print(mymap.get(s0))
    mymap[s0] = step;
    trace[s0] = s0;
    print(mymap.get(s0))
    #print(s0.bv)
    
    q.append(s0);
    qq.append(s0);
    
    i,j = 0,0;
    cnt = 0;
    itr = 0;
    print(">> Begin:")
    
    ###################### A*star Loop #######################

    while(len(q) !=0):
        
        itr = itr+1;
        curr = heappop(q);    
        #curr2 = qq.pop(0);
        
        i = int(curr.pp/width);
        j = mod(curr.pp,width)
        step = mymap[curr];
        previ,prevj = int(trace[curr].pp/width),mod(trace[curr].pp,width);
             
        # solution is found!
        if(sum(curr.bv) == 0):
            retval = mymap[curr]; # cost to the final dot
            print("%s nodes expanded" % itr);
            ii = len(targets)-1;
            while((i,j) != start):# backtrack to generate path 
                if(maze[i][j]=='.'):
                    maze[i][j]=idx_table[ii]; # mark the order of visit
                    ii = ii -1;
                #else:
                    #maze[i][j] = '*';
                curr = trace[curr];
                i,j = int(curr.pp/width),mod(curr.pp,width);
            maze[start[0]][start[1]] = 'P'
            display(maze)
            return retval;# Solution is found
        
        # Go UP
        iu,ju = i-1,j;
        if((maze[iu][ju] != '%')):   
            temp = list(curr.bv);
            if(maze[iu][ju] == '.'):
                cnt = cnt + 1;
                idx = targets.index((iu,ju));
                #display(maze)             
                temp[idx] = 0;
            su = newAstarstate(iu*width+ju,tuple(temp),0,step+1);
            if(mymap.get(su) == None):
                #print("pushing %s,%s,%s" % (iu,ju,su.bv))
                su.h = h_function(dist_matrix,maze,su,targets,n_targets,huge_matrix);
                mymap[su] = step+1;
                trace[su] = curr;
                heappush(q,su);
                #qq.append(su);
                
         # Go RIGHT
        ir,jr = i,j+1;
        if((maze[ir][jr] != '%')):          
            temp = list(curr.bv);
            if(maze[ir][jr] == '.'):
                cnt = cnt + 1;
                idx = targets.index((ir,jr));
                #display(maze)             
                temp[idx] = 0;
            sr = newAstarstate(ir*width+jr,tuple(temp),0,step+1)
            if(mymap.get(sr) == None):
                #print("pushing %s,%s,%s" % (ir,jr,sr.bv))
                sr.h = h_function(dist_matrix,maze,sr,targets,n_targets,huge_matrix);
                mymap[sr] = step+1;
                trace[sr] = curr;
                heappush(q,sr);
                #qq.append(sr);
        
         # Go DOWN
        idd,jd = i+1,j;
        if((maze[idd][jd] != '%')):          
            temp = list(curr.bv);
            if(maze[idd][jd] == '.'):
                cnt = cnt + 1;
                idx = targets.index((idd,jd));
                #display(maze)             
                temp[idx] = 0;               
            sd = newAstarstate(idd*width+jd,tuple(temp),0,step+1)
            if(mymap.get(sd) == None):
                #print("pushing %s,%s,%s" % (idd,jd,sd.bv))
                sd.h = h_function(dist_matrix,maze,sd,targets,n_targets,huge_matrix);
                mymap[sd] = step+1;  
                trace[sd] = curr;
                heappush(q,sd);
                #qq.append(sd);
                
         # Go LEFT
        il,jl = i,j-1;
        if((maze[il][jl] != '%')):          
            temp = list(curr.bv);
            if(maze[il][jl] == '.'):
                cnt = cnt + 1;
                idx = targets.index((il,jl));
                #display(maze)             
                temp[idx] = 0;           
            sl = newAstarstate(il*width+jl,tuple(temp),0,step+1)
            if(mymap.get(sl) == None):
                #print("pushing %s,%s,%s" % (il,jl,sl.bv))
                sl.h = h_function(dist_matrix,maze,sl,targets,n_targets,huge_matrix);
                mymap[sl] = step+1;  
                trace[sl] = curr;
                heappush(q,sl);
                #qq.append(sl);


# In[part1.2]
# I/O
file_name = 'mediumSearch.txt';
#file_name = 'smallSearch.txt';
#file_name = 'tinySearch.txt';

with open(file_name, 'r') as myfile:
    data2 = myfile.read()    
    
    
maze2 = [[x for x in line] for line in data2.split('\n')];
#print(maze);

# Initialize
start = [(index, row.index('P')) for index, row in enumerate(maze2) if 'P' in row]
print("Initial Position at %s\n%s\n" %(start,data2))

start_time = time.clock();

#cost = eatAll_BFS(maze2,start[0]);
cost = eatAll_Astar(maze2,start[0]);

running_time = time.clock()-start_time;

print("Cost is %s" % cost)
if(running_time > 1):
    print("Run time is %s sec" % (running_time))
else:
    print("Run time is %s msec" % (running_time*1000))
    
# Display result
#maze_sol = result["maze_sol"];
#display(maze_sol);

height = len(maze2);
width  = len(maze2[0]);
#print("Cost of solution is %s, %s out of %s nodes expanded!" %(result["cost"],result["n_node"],height*width))
