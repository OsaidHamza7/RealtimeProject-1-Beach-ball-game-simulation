#include "header.h"
#include <math.h>


// Player positions for red and blue teams
float playerPositions[2][6][2] = {
    {{0.3, 0.0}, {0.45, -0.35}, {0.6, -0.5}, {0.75, 0.0}, {0.6, 0.5}, {0.45, 0.35}}, // Red team
    {{-0.3, 0.0}, {-0.45, -0.35}, {-0.6, -0.5}, {-0.75, 0.0}, {-0.6, 0.5}, {-0.45, 0.35}} // Blue team
};
// Global variables for ball positions
float ballPositions[2][2] = {{0.0, 0.0}, {0.0, 0.0}}; // x, y positions for both balls
float interpolationSpeed = 0.02; // Speed of the ball's movement towards the target


// Global variables for ball animation
int currentTarget[2] = {0, 0}; // Start with the first player after the leader
bool moveToLeader[2] = {false, false}; // Start moving towards players first

// Existing drawPlayer, drawHalfCircle, and other functions...
void drawPlayer(float x, float y, float r, float g, float b) {
    glColor3f(r, g, b); // Set player color
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the player
    for (int i = 0; i <= 360; i++) {
        float angle = i * M_PI / 180;
        float dx = x + 0.025 * cos(angle); // Adjust the radius as needed
        float dy = y + 0.05 * sin(angle);
        glVertex2f(dx, dy);
    }
    glEnd();
}

void drawHalfCircle(float x, float y, int segments, float startAngle, float endAngle) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + i * (endAngle - startAngle) / segments;
        float dx = x + 0.07 * cos(angle);
        float dy = y + 0.17 * sin(angle);
        glVertex2f(dx, dy);
    }
    glEnd();
}
void drawBall(float x, float y) {
    glColor3f(0.5, 0.5, 0.5); // Set ball color to gold
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the ball
    for (int i = 0; i <= 360; i++) {
        float angle = i * M_PI / 180;
        float dx = x + 0.019* cos(angle); // Radius adjusted for ball size
        float dy = y + 0.04 * sin(angle);
        glVertex2f(dx, dy);
    }
    glEnd();
}
void drawText1(const char *text, float x, float y) {
    glColor3f(1.0, 0.0, 0.0); // Set text color to white
    glRasterPos2f(x, y); // Position the text on the screen
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); // Display each character
    }
}
void drawText2(const char *text, float x, float y) {
    glColor3f(0.0, 0.0, 1.0); // Set text color to white
    glRasterPos2f(x, y); // Position the text on the screen
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); // Display each character
    }
}



void updateBallPosition(int value) {
    for (int team = 0; team < 2; team++) {
        float targetX = playerPositions[team][currentTarget[team]][0] + (team == 0 ? 0.06 : -0.06);
        float targetY = playerPositions[team][currentTarget[team]][1];
        float directionX = targetX - ballPositions[team][0];
        float directionY = targetY - ballPositions[team][1];
        float length = sqrt(directionX * directionX + directionY * directionY);

        // Normalize direction
        if (length != 0) {
            directionX /= length;
            directionY /= length;
        }

        // Move ball towards the target
        ballPositions[team][0] += directionX * interpolationSpeed;
        ballPositions[team][1] += directionY * interpolationSpeed;

        // Check if ball has reached the target (within a small threshold)
        if (length < interpolationSpeed) {
            // Change speed randomly for next movement
            interpolationSpeed = 0.01 + (float)rand() / RAND_MAX * 0.03; // Random speed between 0.01 and 0.04

            if (moveToLeader[team]) {
                // Move ball back to the leader
                currentTarget[team] = 0;
                moveToLeader[team] = false;
            } else {
                // Move ball to the next player
                currentTarget[team]++;
                if (currentTarget[team] > 5) {
                    moveToLeader[team] = true;
                    currentTarget[team] = 0;
                }
            }
        }
    }

    glutPostRedisplay(); // Redraw the scene with new positions
    glutTimerFunc(16, updateBallPosition, 0); // Continue updating at a high frequency for smooth animation
}


void display() {
    // Clear the screen and draw the field...
    // Draw players...
    glClear(GL_COLOR_BUFFER_BIT);
      // Set line width
    glLineWidth(5.0);

    // Draw the grass (green rectangle)
    glColor3f(0.0, 0.45, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();

    // Set color to white for the lines
    glColor3f(1.0, 1.0, 1.0);

    // Draw the center circle, goal areas, and other field markings...
    // Abbreviated for brevity
     // Draw the center circle
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float angle = i * M_PI / 180;
        float x = 0.0 + 0.15 * cos(angle);
        float y = 0.0 + 0.25 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    // Draw the goal areas, midline, etc.
    // This part of your code was abbreviated for brevity
    //Draw the right goal area big rectangle
   glBegin(GL_LINE_LOOP);
        glVertex2f(0.95, 0.55);  // Top-right corner
        glVertex2f(0.65, 0.55);  // Top-right corner, extended to the right
        glVertex2f(0.65, -0.55);  // Bottom-right corner, extended to the right
        glVertex2f(0.95, -0.55);  // Bottom-right corner
    glEnd();

    // Draw the right goal area small rectangle
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.95, 0.25);
        glVertex2f(0.85, 0.25);
        glVertex2f(0.85, -0.25);
        glVertex2f(0.95, -0.25);
    glEnd();

    //-------------------------------------
    //Left big rectangle
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.95, 0.55);   // Top-left corner
        glVertex2f(-0.65, 0.55);   // Top-left corner, extended to the left
        glVertex2f(-0.65, -0.55);  // Bottom-left corner, extended to the left
        glVertex2f(-0.95, -0.55);  // Bottom-left corner
    glEnd();

    // Draw the left goal area small rectangle
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.95, 0.25);
        glVertex2f(-0.85, 0.25);
        glVertex2f(-0.85, -0.25);
        glVertex2f(-0.95, -0.25);
    glEnd();

    //BIG ALL RECTANGLE
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.95, 0.95);   // Top-left corner
        glVertex2f(0.95, 0.95);   // Top-left corner, extended to the left
        glVertex2f(0.95, -0.95);  // Bottom-left corner, extended to the left
        glVertex2f(-0.95, -0.95);  // Bottom-left corner
    glEnd();


    // Draw the line in the middle
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.95);
    glVertex2f(0.0, -0.95);
    glEnd();

    //Draw left half circle
    //drawHalfCircle(-0.65, 0.0, 180, M_PI / 2, -M_PI / 2);

    //Draw right half circle
    //drawHalfCircle(0.65, 0.0, 180, -M_PI / 2, M_PI / 2);


    for (int team = 0; team < 2; team++) {
        for (int player = 0; player < 6; player++) {
            float playerX = playerPositions[team][player][0];
            float playerY = playerPositions[team][player][1];
            if(player == 0){
                drawPlayer(playerX, playerY, 1.0, 1.0, 0.0);// Draw players with team colors
            }else{
                drawPlayer(playerX, playerY, team == 0 ? 1.0 : 0.0, 0.0, team == 1 ? 1.0 : 0.0); // Draw players with team colors
            }
        }
    }

    // Draw the ball for each team at the appropriate position
    // Inside the display function, replace the drawBall calls with:
    drawBall(ballPositions[0][0], ballPositions[0][1]); // For red team
    drawBall(ballPositions[1][0], ballPositions[1][1]); // For blue team


    drawText2("Score: 0", -0.9, 0.85); // Display near the top-left corner for one team
    drawText1("Score: 0", 0.7, 0.85); // Display near the top-right corner for the other team

    drawText2("Team A", -0.9,-0.9); // Display near the bottom-left corner for one team
    drawText1("Team B", 0.7, -0.9); // Display near the bottom-right corner for the other team


    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Set display mode
    glutInitWindowSize(1100, 600); // Set window size
    glutCreateWindow("Football Field"); // Create window with title

    glClearColor(1.0, 1.0, 1.0, 1.0); // Set clear color to white

    srand(time(NULL)); // Seed the random number generator

    // Register callback functions
    glutDisplayFunc(display); // Register display callback function
    glutTimerFunc(1000, updateBallPosition, 0); // Start the ball movement animation with the timer callback function

    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}