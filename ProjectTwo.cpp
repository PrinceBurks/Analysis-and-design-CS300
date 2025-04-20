#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

// Trim whitespace from both ends of a string
auto Trim = [](const string &s)
{
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? string() : s.substr(start, end - start + 1);
};

// Represents a single course with prerequisites
struct Course
{
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

// Binary Search Tree to store Courses, sorted by courseNumber
class BinarySearchTree
{
private:
    struct Node
    {
        Course course;
        Node *left;
        Node *right;
        Node(const Course &c) : course(c), left(nullptr), right(nullptr) {}
    };
    Node *root = nullptr;

    // Function to insert The Nodes
    Node *insertNode(Node *node, const Course &course)
    {
        if (!node)
            return new Node(course);
        if (course.courseNumber < node->course.courseNumber)
            node->left = insertNode(node->left, course);
        else if (course.courseNumber > node->course.courseNumber)
            node->right = insertNode(node->right, course);
        return node;
    }

    // Traverse the BST in order down the left then right
    void inOrder(Node *node) const
    {
        if (!node)
            return;
        inOrder(node->left);
        cout << node->course.courseNumber << "\t" << node->course.courseName << '\n';
        inOrder(node->right);
    }

    // Fuction to search a specific node by corse Number
    Node *searchNode(Node *node, const string &courseNumber) const
    {
        if (!node)
            return nullptr;
        if (courseNumber == node->course.courseNumber)
            return node;
        return (courseNumber < node->course.courseNumber)
                   ? searchNode(node->left, courseNumber)
                   : searchNode(node->right, courseNumber);
    }

public:
    void Insert(const Course &course) { root = insertNode(root, course); }
    void PrintAll() const
    {
        if (!root)
        {
            cerr << "Error: No courses loaded. Please load data first." << endl;
            return;
        }
        inOrder(root);
    }
    Course *Search(const string &courseNumber) const
    {
        Node *node = searchNode(root, courseNumber);
        return node ? &node->course : nullptr;
    }
};

// Load course data from file into the BST
void LoadCourses(const string &fileName, BinarySearchTree &tree)
{
    ifstream file(fileName);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file '" << fileName << "'.\n"
             << "Make sure the file exists and the path is correct." << endl;
        return;
    }

    string line;
    getline(file, line); // skip the header
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        Course course;
        string field;
        getline(ss, field, ',');
        course.courseNumber = Trim(field);
        getline(ss, field, ',');
        course.courseName = Trim(field);
        while (getline(ss, field, ','))
        {
            field = Trim(field);
            if (!field.empty())
                course.prerequisites.push_back(field);
        }
        tree.Insert(course);
    }
    cout << "Data loaded from '" << fileName << "'." << endl;
}

// Display detailed info for a specific course
void PrintCourseInfo(const BinarySearchTree &tree)
{
    cout << "Enter course number: ";
    string courseNum;
    cin >> courseNum;
    courseNum = Trim(courseNum);

    Course *course = tree.Search(courseNum);
    if (!course)// If entered course Number doesn't exist
    {
        cerr << "Error: Course '" << courseNum << "' not found." << endl;
        return;
    }

    cout << course->courseNumber << " - " << course->courseName << '\n';
    if (course->prerequisites.empty())
    {
        cout << "  Prerequisites: None" << endl;// if there are no prerequisites
    }
    else
    {
        cout << "  Prerequisites:" << endl;
        for (const auto &prereq : course->prerequisites)
        {
            Course *pre = tree.Search(prereq);
            cout << "    " << prereq << " - "
                 << (pre ? pre->courseName : string("[Title not found]")) << '\n';// Handle no title occurances
        }
    }
}

// Main function to handle user input and functionality
int main()
{
    BinarySearchTree tree;
    bool dataLoaded = false;
    while (true)
    { // Display menu to user
        cout << "\n=== ABCU Course Planner ===\n"
             << "1. Load course data\n"
             << "2. Print all courses\n"
             << "3. Print course information\n"
             << "9. Exit\n"
             << "Select an option: ";

        int choice;
        if (!(cin >> choice))
        { // Ensure a valid input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Error: Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice)
        { // Create case for different functionality based on user choice
        case 1:
        {
            cout << "Enter filename: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string filename;
            getline(cin, filename);
            filename = Trim(filename);
            LoadCourses(filename, tree);
            dataLoaded = true;
            break;
        }
        case 2:
            if (!dataLoaded)
                cerr << "Error: No data loaded. Use option 1 first." << endl; // Make sure data was loaded in first
            else
                tree.PrintAll(); // Print all of the info
            break;
        case 3:
            if (!dataLoaded)
                cerr << "Error: No data loaded. Use option 1 first." << endl; // Make sure data was loaded in first
            else
                PrintCourseInfo(tree); // Print the course info
            break;
        case 9:
            cout << "Exiting program. Goodbye!" << endl; // end the program
            return 0;
        default:
            cerr << "Error: Invalid selection. Please choose 1, 2, 3, or 9." << endl; // Re-prompt the user incase of error
        }
    }
    return 0;
}
