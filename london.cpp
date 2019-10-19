//
// london.cpp 
//
//  Fundamental graph algorithms, experiments with the London rail network
//

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include "util.h"

#define INSTANTIATE_MODEL
#include "model.h"

void breadth_first_search( Station start, Station goal );
void depth_first_search( Station start, Station goal );

struct Vertex
{
    Station s;
    Vertex *parent = NULL;
    bool discovered=false;
    Vertex( Station t ) : s(t) {}
};

struct Graph
{
    std::vector<Vertex> vertexes;
    Graph();
    std::set<Station> neighbours( Station s );
    void show_route( Station start, Station goal );
};

int main()
{
    // Loop generating routes
    for(;;)
    {
        Station start, stop;
        int state=0;
        while( state < 4 )
        {
            switch( state )
            {
                case 0: std::cout << "Enter a start station:\n";  break;
                case 1: std::cout << "Start station not found, try again\n";   break;
                case 2: std::cout << "Enter a stop station:\n";   break;
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
        std::cout << "\n";

        // Breadth first search
        std::cout << "Breadth first search\n";
        breadth_first_search( start, stop );

        // Depth first search
        std::cout << "Depth first search\n";
        depth_first_search( start, stop );
    }
    return 0;
}


//
//  Breadth first:
//  One line summary of algorithm:
//
//      Read a queue of discovered vertexes, look at neighbours, add new discoveries only
//
//      Shorter still: Add newly discovered neighbours to queue
//

void breadth_first_search( Station start, Station goal )
{
    Graph g;

    // A queue of discovered vertexes
    std::deque<Station> q;
    g.vertexes[start].discovered = true;
    q.push_back( start );
    while( q.size() > 0 )
    {

        // Dequeue
        Station s = q[0];
        q.pop_front();
        Vertex &v = g.vertexes[s];

        // Reached goal ?
        if( s == goal )
        {
            g.show_route( start, goal );
            return;  // success!
        }

        // Get neighbours
        std::set<Station> neighbours = g.neighbours(s);
        for( Station t: neighbours )
        {

            // Add undiscovered neighbours to the queue
            Vertex &w = g.vertexes[t];
            if( !w.discovered )
            {
                w.discovered = true;
                w.parent = &v;
                q.push_back(w.s);
            }
        }
    }
}

//
//  Depth first:
//  One line summary of algorithm:
//
//      Pop from a stack of vertexes, push all neighbours of new discoveries
//
//      Shorter still: Add all neighbours of new discoveries to stack
//

void depth_first_search( Station start, Station goal )
{
    Graph g;

    // A stack of vertexes
    std::vector<Station> q;
    q.push_back( start );
    while( q.size() > 0 )
    {

        // Pop a vertex
        Station s = q[q.size()-1];
        q.pop_back();
        Vertex &v = g.vertexes[s];
        
        // If it's undiscovered, mark it discovered and push all its neighbours
        if( !v.discovered )
        {
            v.discovered = true;
            std::set<Station> neighbours = g.neighbours(s);
            for( Station t: neighbours )
            {
                Vertex &w = g.vertexes[t];
                if( !w.discovered )
                {
                    w.parent = &v;  // mark parent here because we won't know parent when we pop it later

                    // Found?                
                    if( w.s == goal )
                    {
                        g.show_route( start, goal );
                        return;  // success!
                    }
                }
                q.push_back(w.s);
            }
        }
    }
}

Graph::Graph()
{
    for( int i=0; i<NBR_STATIONS; i++ )
    {
        Station s = static_cast<Station>(i);
        Vertex v(s);
        vertexes.push_back(v);
    }
}

std::set<Station> Graph::neighbours( Station s )
{
    std::set<Station> ret;
    for( int i=0; i<NBR_CONNECTIONS; i++ )
    {
        Connection &c = connections[i];
        if( c.s == s )
            ret.insert(c.t);
    }
    return ret;
}

void Graph::show_route( Station start, Station goal )
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
    for( auto it = path.rbegin(); it != path.rend(); it++ )
    {
        Connection c = *it;
        std::cout << lookup_station[c.s] << " to " << lookup_station[c.t] << " on " << lookup_line[c.l] << "\n";
    }
    std::cout << "\n";
}


