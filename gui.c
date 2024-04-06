#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time(), to seed the random number generator

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to draw the Palestinian flag
void drawPalestinianFlag(float x, float y, float width, float height) {
    // Draw the black stripe
    drawRectangle(x, y + 2 * (height / 3), width, height / 3, 0.0f, 0.0f, 0.0f);
    // Draw the white strip
    drawRectangle(x, y + (height / 3), width, height / 3, 1.0f, 1.0f, 1.0f);
    // Draw the green stripe
    drawRectangle(x, y, width, height / 3, 0.0f, 0.5f, 0.0f);
    // Draw the red triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(x, y);
    glVertex2f(x, y + height);
    glVertex2f(x + width / 2, y + height / 2);
    glEnd();
}

void drawSouthAfricanFlag(float x, float y, float width, float height) {
    // Draw three horizontal rectangles (blue, white, red)
    drawRectangle(x, y, width, height / 3, 0.0f, 0.0f, 0.50f); // Blue
    drawRectangle(x, y + height / 3, width, height / 3, 1.0f, 1.0f, 1.0f); // White
    drawRectangle(x, y + 2 * height / 3, width, height / 3, 0.8f, 0.0f, 0.0f); // Red

    // Draw the large white triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glVertex2f(x, y);
    glVertex2f(x, y + height);
    glVertex2f(x + 3 * width / 5, y + height / 2);  
    glEnd();
    drawRectangle(x, y + height / 2.5, width, height / 5, 0.0f, 0.5f, 0.0f); 

   glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.5f, 0.0f); // Green
    glVertex2f(x, y + height / 15);
    glVertex2f(x, y + 14 * height / 15);
    glVertex2f(x + 11 * width / 20, y + height / 2);  
    glEnd();

    // Draw the yellow triangle, same size as before
    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.8f, 0.0f); // Yellow
    glVertex2f(x, y + height / 5);
    glVertex2f(x, y + 4 * height / 5);
    glVertex2f(x + width / 3, y + height / 2);  
    glEnd();

    // Draw the black triangle
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f); 
    glVertex2f(x, y + 3 * height / 10);
    glVertex2f(x, y + 7 * height / 10);
    glVertex2f(x + width / 4, y + height / 2);  
    glEnd();
}


// Player positions for red and blue teams
float playerPositions[2][6][2] = {
    {{0.3, 0.0}, {0.45, -0.35}, {0.6, -0.5}, {0.75, 0.0}, {0.6, 0.5}, {0.45, 0.35}}, // Red team
    {{-0.3, 0.0}, {-0.45, -0.35}, {-0.6, -0.5}, {-0.75, 0.0}, {-0.6, 0.5}, {-0.45, 0.35}} // Blue team
};
// Global variables for ball positions
int ballTeam[2] = {0, 1}; // 0 for Red team, 1 for Blue team
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

void drawBall(float x, float y, float r, float g, float b) {
    glColor3f(r, g, b); // Set ball color to gold
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

void drawText(const char *text, float x, float y, float r, float g, float b) {
    glColor3f(r, g, b); // Set text color
    glRasterPos2f(x, y); // Position the text on the screen
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); // Display each character
    }
}


void updateBallPosition(int ball);

void switchBallTeam(int ball) {
    ballTeam[ball] = 1 - ballTeam[ball]; // Switch the ball to the other team
    currentTarget[ball] = 0; // Reset target player for the new team
    // Reset ball position (optional based on your game's logic)
    ballPositions[ball][0] = 0.0;
    ballPositions[ball][1] = 0.0;
    // Immediately update the ball position for the new team
    updateBallPosition(ball);
}

void updateBallPosition(int ball) {
    int team = ballTeam[ball];
    float targetX = playerPositions[team][currentTarget[ball]][0] + (team == 0 ? 0.06 : -0.06); // Adjust based on team
    float targetY = playerPositions[team][currentTarget[ball]][1];
    float directionX = targetX - ballPositions[ball][0];
    float directionY = targetY - ballPositions[ball][1];
    float length = sqrt(directionX * directionX + directionY * directionY);
    float speed = 0.01 + (float)rand() / (RAND_MAX / 0.03); // Random speed between 0.01 and 0.04

    if (length > 0) {
        directionX /= length; // Normalize direction vector
        directionY /= length; // Normalize direction vector
    }

    ballPositions[ball][0] += directionX * speed; // Use random speed for movement
    ballPositions[ball][1] += directionY * speed; // Use random speed for movement

    if (length < speed) { // If the ball has reached (or is about to reach) the target
        currentTarget[ball]++;
        if (currentTarget[ball] >= 6) { // Check if the ball needs to switch teams
            switchBallTeam(ball);
            return; // Exit to avoid further processing
        }
    }

    glutPostRedisplay(); // Request a redraw
    glutTimerFunc(16, updateBallPosition, ball); // Schedule the next update
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
    drawBall(ballPositions[0][0], ballPositions[0][1], 0.0, 0.0, 0.0); // For red team
    drawBall(ballPositions[1][0], ballPositions[1][1], 0.5, 0.5, 0.5); // For blue team

    drawPalestinianFlag(0.15, 0.75, 0.15, 0.15);
    drawSouthAfricanFlag(-0.3, 0.75, 0.15, 0.15);

    drawText("0", -0.1, 0.8, 0.0, 0.0, 1.0);
    drawText("0", 0.07, 0.8, 1.0, 0.0, 0.0);

    drawText("Team A", -0.9, -0.9, 0.0, 0.0, 1.0); // Display near the bottom-left corner for the other team
    drawText("Team B", 0.7, -0.9, 1.0, 0.0, 0.0); // Display near the bottom-right corner for the other team

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
    glutTimerFunc(1000, updateBallPosition, 0); // For ball 0
    glutTimerFunc(1000, updateBallPosition, 1); // For ball 1
    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}
