#include <bits/stdc++.h>
using namespace std;

const int INF = numeric_limits<int>::max();

struct Flight {
    int dest;
    int cost;
    double duration;
    double deparTime;
};

struct City {
    int id;
    vector<Flight> flights;
};

class FlightNetwork {
public:
    FlightNetwork(int numCities) : cities(numCities) {}

    void addFlight(int source, int dest, int cost, double duration, double deparTime) {
        cities[source].flights.push_back({dest, cost, duration, deparTime});
    }

    bool removeFlight(int source, int dest, double deparTime) {
        auto& flights = cities[source].flights;
        vector<Flight> upFlights;
        bool rem= false;

        for (const Flight& f : flights) {
            if (f.dest == dest && f.deparTime == deparTime) {
                rem = true;
            } else {
                upFlights.push_back(f);
            }
        }

        flights = upFlights;
        return rem;
    }

    bool bookFlight(int source, int dest, double deparTime) {
        auto& flights = cities[source].flights;
        vector<Flight> upFlights;
        bool bk = false;

        for (const Flight& f : flights) {
            if (!bk && f.dest == dest && f.deparTime == deparTime) {
                bk = true;
            } else {
                upFlights.push_back(f);
            }
        }

        flights = upFlights;
        return bk;
    }

    vector<int> dijkstra(int start, int end) {
        vector<int> dist(cities.size(), INF);
        vector<int> prev(cities.size(), -1);
        dist[start] = 0;

        using pii = pair<int, int>;
        priority_queue<pii, vector<pii>, greater<pii>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (u == end) break;

            for (const Flight& flight : cities[u].flights) {
                int v = flight.dest;
                int weight = flight.cost;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        vector<int> path;
        for (int i = end; i != -1; i = prev[i])
            path.push_back(i);
        reverse(path.begin(), path.end());

        if (path.size() == 1 && path[0] != start)
            return {}; // No path found

        return path;
    }

    vector<Flight> availableFlights(int city) {
        return cities[city].flights;
    }

    void displayFlights(const vector<Flight>& flights) {
        for (size_t i = 0; i < flights.size(); ++i) {
            const Flight& flight = flights[i];
            cout << i + 1 << ". To city " << flight.dest << ", Cost: " << flight.cost << ", Duration: " << flight.duration << ", Departure time: " << flight.deparTime << "\n";
        }
    }

    vector<Flight> scheduleFlights(int city) {
        vector<Flight> allFlights = availableFlights(city);

        // Sort flights by end time (departure_time + duration)
        sort(allFlights.begin(), allFlights.end(), [](const Flight& a, const Flight& b) {
            return (a.deparTime + a.duration) < (b.deparTime + b.duration);
        });

        // Greedy algorithm to select the maximum number of non-overlapping flights
        vector<Flight> schFlights;
        double currTime = 0;

        for (const Flight& flight : allFlights) {
            if (flight.deparTime >= currTime) {
                schFlights.push_back(flight);
                currTime = flight.deparTime + flight.duration;
            }
        }
        return schFlights;
    }

private:
    vector<City> cities;
};

int searchFlight(const vector<Flight>& flights, double time) {
    int left = 0, right = flights.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (flights[mid].deparTime == time)
            return mid;
        if (flights[mid].deparTime < time)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return left; // Nearest flight after the specified time
}

void displayMenu() {
    cout << "Flight Management System Menu:\n";
    cout << "1. Add Flight\n";
    cout << "2. Remove Flight\n";
    cout << "3. Book Flight\n";
    cout << "4. Find Shortest Path\n";
    cout << "5. Display All Flights\n";
    cout << "6. Schedule Flights\n";
    cout << "7. Search for Nearest Flight\n";
    cout << "8. Exit\n";
}

int main() {
    int numCities, numFlights;
    cout << "Enter the number of cities: ";
    cin >> numCities;

    FlightNetwork network(numCities);

    while (true) {
        displayMenu();
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 8) break;

        switch (choice) {
            case 1: {
                int source, dest, cost;
                double duration, deparTime;
                cout << "Enter flight details (source destination cost duration departure_time): ";
                cin >> source >> dest >> cost >> duration >> deparTime;
                network.addFlight(source, dest, cost, duration, deparTime);
                break;
            }
            case 2: {
                int source, dest;
                double deparTime;
                cout << "Enter flight details to remove (source destination departure_time): ";
                cin >> source >> dest >> deparTime;
                if (network.removeFlight(source, dest, deparTime)) {
                    cout << "Flight removed successfully.\n";
                } else {
                    cout << "Flight not found.\n";
                }
                break;
            }
            case 3: {
                int source, dest;
                double deparTime;
                cout << "Enter flight details to book (source destination departure_time): ";
                cin >> source >> dest >> deparTime;
                if (network.bookFlight(source, dest, deparTime)) {
                    cout << "Flight booked successfully.\n";
                } else {
                    cout << "Flight not found.\n";
                }
                break;
            }
            case 4: {
                int stCity, endCity;
                cout << "Enter the start city and end city: ";
                cin >> stCity >> endCity;
                vector<int> shortestPath = network.dijkstra(stCity, endCity);
                if (shortestPath.empty()) {
                    cout << "No path found from city " << stCity << " to city " << endCity << ".\n";
                } else {
                    cout << "Shortest path from city " << stCity << " to city " << endCity << ": ";
                    for (int city : shortestPath) {
                        cout << city << " ";
                    }
                    cout << "\n";
                }
                break;
            }
            case 5: {
                int city;
                cout << "Enter the city: ";
                cin >> city;
                vector<Flight> allFlights = network.availableFlights(city);
                cout << "All flights from city " << city << ":\n";
                network.displayFlights(allFlights);
                break;
            }
            case 6: {
                int city;
                cout << "Enter the city: ";
                cin >> city;
                vector<Flight> schFlights = network.scheduleFlights(city);
                cout << "Scheduled flights from city " << city << ":\n";
                for (const Flight& flight : schFlights) {
                    cout << "To city " << flight.dest << ", Cost: " << flight.cost << ", Duration: " << flight.duration << ", Departure time: " << flight.deparTime << "\n";
                }
                break;
            }
            case 7: {
                int city;
                double desiredTime;
                cout << "Enter the city and desired departure time: ";
                cin >> city >> desiredTime;
                vector<Flight> allFlights = network.availableFlights(city);
                vector<Flight> avlFlights;

                for (const Flight& flight : allFlights) {
                    if (flight.deparTime >= desiredTime) {
                        avlFlights.push_back(flight);
                    }
                }

                int flightIndex = searchFlight(avlFlights, desiredTime);
                if (flightIndex < avlFlights.size()) {
                    const Flight& flight = avlFlights[flightIndex];
                    cout << "Nearest available flight departs at " << flight.deparTime << " to city " << flight.dest << " with cost " << flight.cost << " and duration " << flight.duration << ".\n";
                } else {
                    cout << "No available flights found after the specified time.\n";
                }
                break;
            }
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}
