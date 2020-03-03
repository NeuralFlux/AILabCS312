## For Node Comparison
graph[current_coord.first][current_coord.second].f = 100;
graph[3][4].f = 101;
graph[4][5].f = 102;
open.push(&graph[3][4]);
open.push(&graph[4][5]);

while(!open.empty()) {
    cout<< "Current: " << open.top()->f <<endl;
    open.pop();
}