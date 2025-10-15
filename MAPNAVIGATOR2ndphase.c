#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_POINTS 21
#define ROAD_FACTOR 2.4  // Rough factor for road distance estimation

typedef struct {
    char name[50];
    double lat;
    double lng;
} Point;

// ==================== FUNCTION PROTOTYPES ====================
double distance(double lat1, double lon1, double lat2, double lon2);
int compare_ignore_case(const char *a, const char *b);
int simple_match_score(const char *input, const char *name);
int find_point_index(const char *name);
void suggest_top3(const char *input, int indices[3]);
int get_landmark(const char *prompt);

// ==================== DEHRADUN DATA ====================
Point points[MAX_POINTS] = {
    {"Clock Tower", 30.3185, 78.0290},
    {"Robber's Cave", 30.3655, 78.0421},
    {"Forest Research Institute", 30.3401, 77.9960},
    {"Sahasradhara", 30.3874, 78.1318},
    {"Tapkeshwar Temple", 30.3440, 78.0066},
    {"Pacific Mall", 30.3446, 78.0531},
    {"ISBT Dehradun", 30.2857, 78.0081},
    {"Mussoorie Road", 30.3794, 78.0859},
    {"Rajpur Road", 30.3707, 78.0758},
    {"Dehradun Zoo", 30.3600, 78.0450},
    {"Malsi Deer Park", 30.2710, 78.0460},
    {"Mindrolling Monastery", 30.3010, 78.0190},
    {"Robbers Cave Road", 30.3658, 78.0430},
    {"Assan Barrage", 30.2310, 78.0500},
    {"Chetwoode Hall", 30.3370, 78.0010},
    {"Clock Tower Market", 30.3190, 78.0295},
    {"Government Museum", 30.3195, 78.0340},
    {"Rajaji National Park Entrance", 30.4380, 78.0700},
    {"Dehradun Railway Station", 30.3200, 78.0320},
    {"Premnagar", 30.3560, 78.0610},
    {"Doon Valley", 30.3240, 78.0380} 
};

// ==================== MAIN ====================
int main() {
    printf("=== Dehradun Map Navigator ===\n\n");

    printf("Available landmarks:\n");
    for (int i = 0; i < MAX_POINTS; i++)
        printf(" - %s\n", points[i].name);

    int currentIndex = get_landmark("\nWhere are you currently? ");
    int destIndex = get_landmark("Enter your destination: ");

    double straight_dist = distance(points[currentIndex].lat, points[currentIndex].lng,
                                    points[destIndex].lat, points[destIndex].lng);
    double road_dist = straight_dist * ROAD_FACTOR;

    printf("\nFrom: %s\n", points[currentIndex].name);
    printf("To: %s\n", points[destIndex].name);
    printf("Straight-line distance: %.2f km\n", straight_dist);
    printf("Estimated road distance: %.2f km\n", road_dist);

    return 0;
}

// ==================== FUNCTIONS ====================

// Haversine formula
double distance(double lat1, double lon1, double lat2, double lon2) {
    double R = 6371.0; // Earth radius in km
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dLat/2) * sin(dLat/2) +
               cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

// Case-insensitive string comparison
int compare_ignore_case(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower(*a) != tolower(*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

// Typo-tolerant match score
int simple_match_score(const char *input, const char *name) {
    int score = 0;
    int len_input = strlen(input);
    int len_name = strlen(name);

    for (int i = 0; i < len_input; i++) {
        char c = tolower(input[i]);
        for (int j = 0; j < len_name; j++) {
            if (c == tolower(name[j])) {
                score++;
                break; // match each input char only once
            }
        }
    }
    return score;
}

// Find landmark by exact name
int find_point_index(const char *name) {
    for (int i = 0; i < MAX_POINTS; i++) {
        if (compare_ignore_case(name, points[i].name)) return i;
    }
    return -1;
}

// Suggest top 3 closest matches
void suggest_top3(const char *input, int indices[3]) {
    int scores[MAX_POINTS];
    for (int i = 0; i < MAX_POINTS; i++)
        scores[i] = simple_match_score(input, points[i].name);

    for (int k = 0; k < 3; k++) {
        int maxScore = -1, maxIndex = -1;
        for (int i = 0; i < MAX_POINTS; i++) {
            if (scores[i] > maxScore) {
                maxScore = scores[i];
                maxIndex = i;
            }
        }
        indices[k] = maxIndex;
        if (maxIndex != -1) scores[maxIndex] = -1; // exclude for next max
    }
}

// Prompt user for valid landmark
int get_landmark(const char *prompt) {
    char input[50];
    int index;
    int top3[3];

    while (1) {
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        index = find_point_index(input);
        if (index != -1) return index;

        // Get top 3 matches
        suggest_top3(input, top3);

        printf(" '%s' not found. Did you mean:\n", input);
        for (int i = 0; i < 3; i++) {
            if (top3[i] != -1)
                printf(" %d) %s\n", i + 1, points[top3[i]].name);
        }
        printf("Enter choice number (or 0 to retry): ");
        int choice;
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice >= 1 && choice <= 3 && top3[choice - 1] != -1)
            return top3[choice - 1];
        else
            printf("Please enter again.\n");
    }
}
