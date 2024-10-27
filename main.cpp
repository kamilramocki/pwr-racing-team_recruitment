//
// Created by Kamil Ramocki on 27/10/2024.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <cmath>

using namespace std;

// Struktura reprezentująca punkt w przestrzeni 2D
struct Point {
    double x, y;

    // Operator porównania dla użycia w unordered_set
    bool operator==(const Point& other) const {
        return abs(x - other.x) < 1e-9 && abs(y - other.y) < 1e-9;
    }
};

// Specjalizacja hashera do użycia Point w unordered_set
namespace std {
    template <>
    struct hash<Point> {
        size_t operator()(const Point& p) const noexcept {
            return hash<double>()(p.x) ^ hash<double>()(p.y);
        }
    };
}

// Funkcja porównująca dwa punkty
bool compare(const Point &p1, const Point &p2) {
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}

// Funkcja obliczająca iloczyn wektorowy trzech punktów
double cross(const Point &O, const Point &A, const Point &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Funkcja budująca otoczkę
vector<Point> convexHull(vector<Point> &points) {
    if (points.size() < 3) {
        cerr << "Error: At least 3 points are required to form a convex hull." << endl;
        return {};
    }

    // Usuwanie duplikatów punktów
    unordered_set<Point> unique_points(points.begin(), points.end());
    points.assign(unique_points.begin(), unique_points.end());

    // Sortowanie punktów według współrzędnych x i y
    ranges::sort(points, compare);

    int n = points.size();
    vector<Point> hull;

    // Budowanie dolnej części otoczki
    for (int i = 0; i < n; ++i) {
        while (hull.size() >= 2 && cross(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0)
            hull.pop_back(); // Usuwanie ostatniego punktu, jeśli tworzy wklęsłą część
        hull.push_back(points[i]); // Dodawanie punktu do otoczki
    }

    // Budowanie górnej części otoczki
    for (int i = n - 2, t = hull.size() + 1; i >= 0; --i) {
        while (hull.size() >= t && cross(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0)
            hull.pop_back(); // Usuwanie ostatniego punktu, jeśli tworzy wklęsłą część
        hull.push_back(points[i]); // Dodawanie punktu do otoczki
    }

    if (!hull.empty()) {
        hull.pop_back(); // Usuwanie ostatniego punktu, ponieważ jest taki sam jak pierwszy
    }

    return hull;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Could not open file." << endl;
        return 1;
    }

    int n;
    if (!(infile >> n) || n <= 0) {  // Sprawdzanie poprawności liczby punktów
        cerr << "Error: Invalid number of points specified." << endl;
        return 1;
    }

    vector<Point> points(n); // Tworzenie wektora punktów
    for (int i = 0; i < n; ++i) {
        if (!(infile >> points[i].x >> points[i].y)) {  // Sprawdzanie poprawności danych punktów
            cerr << "Error: Invalid file format. Expected " << n << " points." << endl;
            return 1;
        }
    }

    if (points.size() < 3) {  // Sprawdzanie, czy mamy co najmniej 3 unikalne punkty
        cerr << "Error: At least 3 unique points are required to form a convex hull." << endl;
        return 1;
    }

    // Obliczenie otoczki
    const vector<Point> hull = convexHull(points);

    // Sprawdzenie czy udało się zbudować otoczkę
    if (hull.empty()) {
        return 1;
    }

    // Wyświetlenie punktów otoczki
    cout << "[";
    for (size_t i = 0; i < hull.size(); ++i) {
        cout << "(" << hull[i].x << ", " << hull[i].y << ")";
        if (i < hull.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;

    return 0;
}