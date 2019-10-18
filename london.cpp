//
// london.cpp Experiments with the London rail network
//

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "util.h"

#define INSTANTIATE_MODEL
#include "model.h"

#define nbrof(x) (sizeof(x)/sizeof(x[0]))


typedef std::vector<Line> Graph[NBR_STATIONS][NBR_STATIONS];

struct Vertex
{
    Station s;
    Vertex *parent = NULL;
    bool discovered=false;
    Vertex( Station t ) : s(t) {}
    bool operator == ( const Vertex &v ) { return (s==v.s); }
};

void show_solution( std::vector<Vertex> &vertexes, Station start, Station goal );

void breadth_first_search( Graph &g, std::vector<Vertex> &vertexes, Station start, Station goal )
{
    vertexes[start].discovered = true;
    std::deque<Station> q;
    q.push_back( start );
    while( q.size() >= 0 )
    {
        Station s = q[0];
        q.pop_front();
        Vertex &v = vertexes[s];

        // Found ?
        if( s == goal )
        {
            show_solution( vertexes, start, goal );
            return;  // success!
        }
        for( int i=0; i<NBR_STATIONS; i++ )
        {
            if( g[v.s][i].size() > 0 )
            {
                Vertex &w = vertexes[i];
                if( !w.discovered )
                {
                   w.discovered = true;
                   w.parent = &v;
                   q.push_back(w.s);
                }
            }
        }
    }
}

void depth_first_search( Graph &g, std::vector<Vertex> &vertexes, Station start, Station goal )
{
    std::vector<Station> q;
    q.push_back( start );
    while( q.size() >= 0 )
    {
        Station s = q[q.size()-1];
        q.pop_back();
        Vertex &v = vertexes[s];
        if( !v.discovered )
        {
            v.discovered = true;
            for( int i=0; i<NBR_STATIONS; i++ )
            {
                if( g[v.s][i].size() > 0 )
                {
                    Vertex &w = vertexes[i];
                    if( !w.discovered )
                        w.parent = &v;
                    q.push_back(w.s);
                    std::cout << lookup_station[v.s] << " " << lookup_station[w.s] << "\n"; 

                    // Found ?
                    if( w.s == goal )
                    {
                        for( auto it = q.begin(); it != q.end(); it++ )
                            std::cout << lookup_station[*it] << "\n"; 

                        show_solution( vertexes, start, goal );
                        return;  // success!
                    }
                }
            }
        }
    }
}

void show_solution( std::vector<Vertex> &vertexes, Station start, Station goal )
{
    std::vector<Connection> path;
    Vertex &v = vertexes[goal];
    Line current_line = NBR_LINES; 
    for(;;)
    {
        if( v.parent == NULL )
            return; // something went wrong 
        Connection c;
        c.t = v.s;
        c.s = v.parent->s;
        c.l = NBR_LINES;
        for( Connection d: connections )
        {
            if( d.s==c.s && d.t==c.t )
            {
                c.l = d.l;
                if( c.l == current_line )
                    break;
            }
        }
        if( c.l == NBR_LINES )
            return; // something went wrong
        current_line = c.l;     // try to stay on this line;
        path.push_back( c );
        v = *v.parent;
        if( c.s == start )
            break;
    }
    std::cout << "\n";
    for( auto it = path.rbegin(); it != path.rend(); it++ )
    {
        Connection c = *it;
        std::cout << lookup_station[c.s] << " to " << lookup_station[c.t] << " on " << lookup_line[c.l] << "\n";
    }
    std::cout << "\n";
}


int main()
{

    // Sparse matrix
    static Graph matrix;

    // Put each connection into the sparse matrix
    for( int i=0; i<nbrof(connections); i++ )
    {
        Connection &c = connections[i];
        Station s = c.s;
        Station t = c.t;
        Line    l = c.l;
        std::vector<Line> &v = matrix[s][t]; 
        v.push_back(l);
    }

    // Loop generating routes
    for(;;)
    {

        // Initiate an empty vertext for each station
        std::vector<Vertex> vertexes;
        for( int i=0; i<NBR_STATIONS; i++ )
        {
            Station s = static_cast<Station>(i);
            Vertex v(s);
            vertexes.push_back(v);
        }

        Station start, stop;
        int state=0;
        while( state < 4 )
        {
            switch( state )
            {
                case 0: std::cout << "Enter a start station\n";  break;
                case 1: std::cout << "Start station not found, try again\n";   break;
                case 2: std::cout << "Enter a stop station\n";   break;
                case 3: std::cout << "Stop station not found, try again\n";   break;
            }
            std::string txt;
            std::getline( std::cin, txt );
            util::rtrim(txt);
            txt = util::tolower(txt);
            if( txt=="" || txt == "q" || txt=="quit" )
                return 0;
            int old_state = state;
            for( int i=0; i<NBR_STATIONS; i++ )
            {
                std::string s(lookup_station[i]);
                s = util::tolower(s);
                if( txt == s )
                {
                    if( state <= 1 )
                    {
                        start = static_cast<Station>( i ); 
                        state = 2;
                    }
                    else
                    {
                        stop = static_cast<Station>( i ); 
                        state = 4;
                    }
                    break;
                }
            }
            if( old_state==state && state%2==0 )
                state++;
        }

    /*    // Breadth first search
        breadth_first_search( matrix, vertexes, start, stop );

        // Initiate an empty vertext for each station
        vertexes.clear();
        for( int i=0; i<NBR_STATIONS; i++ )
        {
            Station s = static_cast<Station>(i);
            Vertex v(s);
            vertexes.push_back(v);
        } */

        // Depth first search
        depth_first_search( matrix, vertexes, start, stop );
    }
    return 0;
}

