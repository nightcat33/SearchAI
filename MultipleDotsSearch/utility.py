# define return structure as follow:
# 
# {maze_sol,cost,n_node}
# In[testdfs]
def display(maze):
    
    height = len(maze);
    width  = len(maze[0]);
    output_matrix = [];
    for i in range(height):
        #result["maze_sol"][i].append('\n')
        output_matrix.append(''.join(maze[i]));
        print(output_matrix[i]);
