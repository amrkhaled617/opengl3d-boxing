#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f &v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f &v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 0.0392753f, float eyeY = 0.555302f, float eyeZ = 0.679326f, float centerX = 0.823861f, float centerY = 0.106974f, float centerZ = 0.251042f, 
		float upX = 0.480039f, float upY = 0.876422f, float upZ = -0.0380423f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}
//Position: (0.0392753, 0.555302, 0.679326)
//Position : (0.823861, 0.106974, 0.251042)
//Position : (0.480039, 0.876422, -0.0380423)
	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
		printCamera();
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
		printCamera();

	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
		printCamera();

	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
		printCamera();

	}


	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
		printCamera();

	}
	void rotateZ(float a) {
		Vector3f view = (center - eye).unit(); // View direction
		Vector3f right = up.cross(view).unit(); // Right vector
		right = right * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		printCamera();
	}
	void topView() {
		// Set the camera above the center looking down (Y-axis)

		
		eye = Vector3f(0.433861, 1.60697, 0.551042);
		center = Vector3f(0.433861, 0.106974, 0.251042);
		//eye = VVector3f(center.x , center.y + 1.5f, center.z + 0.3f);
		up = Vector3f(0.0f, 0.0f, -1.0f); // Up direction pointing towards negative Z-axis
	}

	void sideView() {
		eye = Vector3f(1.67462, 0.805084, 0.529901);
		center = Vector3f(0.692524, 0.618173, 0.506504);
		// Set the camera to the side of the center, along the X-axis
		//eye = Vector3f(center.x + 2.0f, center.y + 0.7f, center.z + 0.5f);
		up = Vector3f(0.0f, 1.0f, 0.0f); // Up direction along the Y-axis
	}

	void frontView() {
		eye = Vector3f(0.390785, 0.889035, 0.925888);
		center = Vector3f(0.383251, 0.264429, 0.144985);
		// Set the camera in front of the center, along the Z-axis
		//eye = Vector3f(center.x +0.3f, center.y + 0.8f , center.z + 0.98f);
		up = Vector3f(0.0f, 1.0f, 0.0f); // Up direction along the Y-axis
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
	void printCamera() {
		std::cout << "Position: (" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
		std::cout << "Position: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
		std::cout << "Position: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
	}
};

Camera camera;
float x, y, z; // Position of the player
float angleY;  // Angle for rotation around the Y-axis (for horizontal rotation)
float speed = 0.01f;   // Movement speed
float wallColor[3] = { 1.0f, 0.0f, 0.0f }; // Initially red
float chairPosY = 0.2f, chairAngle = 0.0f, chairScale = 1.0f;
float bottlePosX = 0.8f, bottleColor[3] = { 0.0f, 0.0f, 1.0f }; // Initial blue
float fanAngle = 0.0f, fanScale = 1.0f;
float dumbbellAngle = 0.0f, dumbbellPosZ = 0.8f;
float playerRotation = 0.0f; // For continuous rotation
float leftArmAngle = 0.0f, rightArmAngle = 0.0f; // For arm swinging
bool leftArmUp = true, rightArmUp = false;
float bagColor[3] = { 1.0f, 0.0f, 0.0f }; // Initial red
bool isAnimating = false;
float playerX = 0.6f;
float playerZ = 0.2f;
int playerDirection = 1;
int currentAngle = 0;
bool isGameWin = false;
time_t startTime = time(NULL); // To track when the game starts
bool isGameLose = false; // To avoid redundant updates
int i=0;
time_t startTime1;


void drawWall(double thickness);

void rotatePlayer() {
	//if (playerDirection == 1) {
	//	currentAngle = 0; // Facing forward
	//}
	//else if (playerDirection == 2) {
	//	currentAngle = 180; // Facing backward
	//}
	//else if (playerDirection == 3) {
	//	currentAngle = 90; // Facing left
	//}
	//else if (playerDirection == 4) {
	//	currentAngle = -90; // Facing right
	//}
}
void animate(int value) {
	if (!isAnimating) return;

	if (i == 0) {
		startTime1 = time(NULL); 
		i++;
	}

	time_t currentTime1 = time(NULL);
	double elapsedSeconds = difftime(currentTime1, startTime1);



	// Chair: Translate up and down, rotate


	// Bottle: Move left and right, change color
	bottlePosX += 0.01f * cos(bottlePosX * M_PI);
	bottleColor[0] = fabs(sin(bottlePosX)); // Red component varies
	bottleColor[1] = fabs(cos(bottlePosX)); // Green component varies

	// Fan: Rotate and scale up and down
	fanAngle += 5.0f;
	if (fanAngle > 360.0f) fanAngle -= 360.0f;
	fanScale = 1.0f + 0.1f * sin(fanAngle * M_PI / 180.0f);

	// Dumbbell: Rotate and translate in Z-axis
	dumbbellAngle += 3.0f;
	if (dumbbellAngle > 360.0f) dumbbellAngle -= 360.0f;
	dumbbellPosZ += 0.01f * sin(dumbbellAngle * M_PI / 180.0f);

	// Punching Bag: Change color dynamically
	bagColor[0] = fabs(sin(dumbbellAngle)); // Red component varies
	bagColor[2] = fabs(cos(dumbbellAngle)); // Blue component varies
	if (elapsedSeconds <= 5) {
		chairAngle += 2.0f;
		if (chairAngle > 360.0f) chairAngle -= 360.0f;
		chairScale += 0.01f;
		chairPosY += 0.01f * sin(chairAngle * M_PI / 180.0f);

	}

	glutPostRedisplay(); // Redraw the scene with updated transformations
	glutTimerFunc(16, animate, 0); // Re-trigger animation every 16ms (~60 FPS)
}
void wallColorChange(int value) {
	// Change the wall color based on a time counter (or randomly, for example)
	static int colorToggle = 0; // Keeps track of which color to choose

	// Choose a color based on the toggle
	if (colorToggle % 2 == 0) {
		wallColor[0] = 0.0f; // Blue
		wallColor[1] = 0.0f;
		wallColor[2] = 1.0f;
	}
	else {
		wallColor[0] = 1.0f; // Red
		wallColor[1] = 0.0f;
		wallColor[2] = 0.0f;
	}
	colorToggle++; // Toggle the color

	// Your code to draw walls with color change
	std::cout << "Changing color every 5 seconds" << std::endl;
	// Re-schedule the method to be called again in 5000 milliseconds (5 seconds)
	glutTimerFunc(5000, wallColorChange, 0);
	glutPostRedisplay();

}
//void drawWall(double thickness) {
//	glPushMatrix();
//	glTranslated(0.5, 0.5 * thickness, 0.5);
//	glScaled(1.0, thickness, 1.0);
//	glColor3f(wallColor[0], wallColor[1], wallColor[2]); // Use global wall color
//	glutSolidCube(1);
//	glPopMatrix();
//}
void drawWall(double thickness) {
	glPushMatrix();

	// First primitive: Cube for the main wall
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5); // Position the cube
	glScaled(1.0, thickness, 1.0);          // Scale the cube
	glColor3f(wallColor[0], wallColor[1], wallColor[2]); // Wall color
	glutSolidCube(1);
	glPopMatrix();

	// Second primitive: Smaller cube as a decorative element
	glPushMatrix();
	glTranslated(0.5, thickness + 0.05, 0.5); // Adjust position to touch the top of the wall
	glScaled(0.1, 0.1, 0.1);                // Scale the smaller cube to 10% of the original size
	glColor3f(0.8, 0.3, 0.3);               // Different color for the small cube
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
}

void drawPunchingBag(float scaleFactor) {
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the base (a scaled cube)
	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f); // Position the base on the ground
	glScalef(1.0f, 0.2f, 1.0f);    // Scale the cube to make a flat base
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the pole (a scaled cube)
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f); // Position the pole above the base
	glScalef(0.1f, 2.0f, 0.1f);    // Scale the cube to make a tall pole
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the hook (a torus)
	glPushMatrix();
	glTranslatef(0.0f, 1.5f, 0.0f); // Position the hook at the top of the pole
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate the torus to align horizontally
	glutSolidTorus(0.05f, 0.2f, 20, 20);
	glPopMatrix();

	// Draw the punching bag (a stretched sphere)
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 0.0f); // Position the bag below the hook
	glScalef(0.4f, 1.0f, 0.4f);    // Stretch the sphere to make it cylindrical
	glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();

	// Draw the bottom detailing of the bag (a smaller sphere)
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f); // Position at the bottom of the bag
	glScalef(0.4f, 0.2f, 0.4f);    // Scale to make it a smaller rounded bottom
	glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawTable(float scaleFactor) {
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the tabletop (a flat cube)
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f); // Position the tabletop above the ground
	glScalef(2.0f, 0.1f, 1.0f);    // Scale the cube to make it a flat tabletop
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the first leg
	glPushMatrix();
	glTranslatef(-0.9f, 0.5f, -0.4f); // Position the first leg under the tabletop
	glScalef(0.1f, 1.0f, 0.1f);       // Scale to make a narrow leg
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the second leg
	glPushMatrix();
	glTranslatef(0.9f, 0.5f, -0.4f); // Position the second leg
	glScalef(0.1f, 1.0f, 0.1f);      // Scale to make a narrow leg
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the third leg
	glPushMatrix();
	glTranslatef(-0.9f, 0.5f, 0.4f); // Position the third leg
	glScalef(0.1f, 1.0f, 0.1f);      // Scale to make a narrow leg
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the fourth leg
	glPushMatrix();
	glTranslatef(0.9f, 0.5f, 0.4f); // Position the fourth leg
	glScalef(0.1f, 1.0f, 0.1f);     // Scale to make a narrow leg
	glutSolidCube(1.0f);
	glPopMatrix();

	glPopMatrix();
}
void drawBottle(float scaleFactor) {
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the base (a scaled cube)
	glPushMatrix();
	glTranslatef(0.0f, 0.2f, 0.0f); // Position the base above the ground
	glScalef(0.6f, 0.4f, 0.6f);    // Scale the cube to make it a flat base
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the body (a scaled sphere to resemble a rounded body)
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 0.0f); // Position the body above the base
	glScalef(0.5f, 1.0f, 0.5f);    // Stretch the sphere to make it bottle-like
	glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();

	// Draw the neck (a scaled cube for simplicity)
	glPushMatrix();
	glTranslatef(0.0f, 1.6f, 0.0f); // Position the neck above the body
	glScalef(0.2f, 0.6f, 0.2f);    // Scale to make a narrow neck
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the bottle opening (a torus for the lip)
	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f); // Position the torus at the top of the neck
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate the torus to be horizontal
	glutSolidTorus(0.05f, 0.2f, 20, 20);
	glPopMatrix();

	// Draw the bottle cap (a small sphere)
	glPushMatrix();
	glTranslatef(0.0f, 2.2f, 0.0f); // Position the cap above the neck
	glScalef(0.3f, 0.2f, 0.3f);    // Scale the sphere to resemble a cap
	glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawFan(float scaleFactor) {
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the base (a scaled cube)
	glPushMatrix();
	glTranslatef(0.0f, 0.1f, 0.0f); // Position the base on the ground
	glScalef(0.4f, 0.2f, 0.4f);    // Scale the cube to make a rectangular base
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the pole (a scaled cube)
	glPushMatrix();
	glTranslatef(0.0f, 0.6f, 0.0f); // Position the pole above the base
	glScalef(0.1f, 1.0f, 0.1f);    // Scale the cube to make a tall pole
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the motor housing (a sphere)
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f); // Position the motor housing on top of the pole
	glutSolidSphere(0.15f, 20, 20);
	glPopMatrix();

	// Draw the fan blades (4 scaled cubes)
	float bladeLength = 0.8f;
	float bladeWidth = 0.1f;

	// Blade 1
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f); // Position the blade at the motor housing
	glScalef(bladeWidth, bladeLength, bladeWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Blade 2
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // Rotate the blade by 90 degrees
	glScalef(bladeWidth, bladeLength, bladeWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Blade 3
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f);
	glRotatef(45.0f, 0.0f, 0.0f, 1.0f); // Rotate the blade by 45 degrees
	glScalef(bladeWidth, bladeLength, bladeWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Blade 4
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f);
	glRotatef(-45.0f, 0.0f, 0.0f, 1.0f); // Rotate the blade by -45 degrees
	glScalef(bladeWidth, bladeLength, bladeWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the center of the fan (a small sphere)
	glPushMatrix();
	glTranslatef(0.0f, 1.2f, 0.0f); // Centered at the motor housing
	glutSolidSphere(0.1f, 20, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawDumbbell(float scaleFactor) {
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the handle (a scaled cube)
	glPushMatrix();
	glScalef(1.5f, 0.2f, 0.2f); // Scale the cube to make a long thin handle
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the left weight (a sphere)
	glPushMatrix();
	glTranslatef(-0.85f, 0.0f, 0.0f); // Position the left weight at one end of the handle
	glutSolidSphere(0.4f, 20, 20);      // Create the rotated sphere
	glPopMatrix();


	// Draw the right weight (a sphere)
	glPushMatrix();
	glTranslatef(0.85f, 0.0f, 0.0f); // Position the right weight at the other end of the handle
	glutSolidSphere(0.4f, 20, 20);
	glPopMatrix();

	// Add detailing rings on the left weight (torus)
	glPushMatrix();
	glTranslatef(-0.85f, 0.0f, 0.0f); // Align with the left weight
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with the sphere
	glutSolidTorus(0.05f, 0.4f, 20, 20);
	glPopMatrix();

	// Add detailing rings on the right weight (torus)
	glPushMatrix();
	glTranslatef(0.85f, 0.0f, 0.0f); // Align with the right weight
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with the sphere
	glutSolidTorus(0.05f, 0.4f, 20, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawChair(float scaleFactor) {
	// Scale the chair based on the scaleFactor
	glPushMatrix();
	glScalef(scaleFactor, scaleFactor, scaleFactor);

	// Draw the seat (cube)
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f); // Position the seat above the ground
	glScalef(1.0f, 0.2f, 1.0f);    // Flatten the cube to make it a seat
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the backrest (rectangle)
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, -0.4f); // Position the backrest behind the seat
	glScalef(1.0f, 1.0f, 0.1f);      // Flatten the cube to make it a backrest
	glutSolidCube(1.0f);
	glPopMatrix();

	// Draw the legs (scaled cubes to approximate cylinders)
	float legHeight = 0.5f;
	float legWidth = 0.1f;

	// Front-left leg
	glPushMatrix();
	glTranslatef(-0.4f, legHeight / 2.0f, 0.4f); // Position the leg
	glScalef(legWidth, legHeight, legWidth);     // Scale the cube to resemble a leg
	glutSolidCube(1.0f);
	glPopMatrix();

	// Front-right leg
	glPushMatrix();
	glTranslatef(0.4f, legHeight / 2.0f, 0.4f);
	glScalef(legWidth, legHeight, legWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Back-left leg
	glPushMatrix();
	glTranslatef(-0.4f, legHeight / 2.0f, -0.4f);
	glScalef(legWidth, legHeight, legWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Back-right leg
	glPushMatrix();
	glTranslatef(0.4f, legHeight / 2.0f, -0.4f);
	glScalef(legWidth, legHeight, legWidth);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPopMatrix();
}
void printPosition() {
	std::cout << "Position: (" << x << ", " << y << ", " << z << ")" << std::endl;
}
void disappearGoal() {
	std::cout << "Goal has been reached!" << std::endl;
	playerX = 1.0f;
	playerZ = 1.0f;
	isGameWin = true;
}
void checkPlayerPosition() {
	if (x > 0.35f && z < -0.35f) {
		std::cout << "Collision with goal" << std::endl;
		x = 0.0f;
		z = 0.0f;
		disappearGoal();
	}
	if (x <= -0.07f) {
		std::cout << "Collision" << std::endl;
		x = 0.0f;
		z = 0.0f;

	}
	if (z >= 0.04f) {
		std::cout << "Collision" << std::endl;
		x = 0.0f;
		z = 0.0f;

	}
	if (z <= -0.45f) {
		std::cout << "Collision" << std::endl;
		x = 0.0f;
		z = 0.0f;
	}
	if (x >= 0.45f) {
		std::cout << "Collision" << std::endl;
		x = 0.0f;
		z = 0.0f;
	}
}
void moveForward() {
	x += speed ;
	rotatePlayer();
	printPosition();
	checkPlayerPosition();
}
void moveBackward() {
	x -= speed ;
	rotatePlayer();
	printPosition();
	checkPlayerPosition();

}
void moveLeft() {
	z += speed ;
	rotatePlayer();

	printPosition();
	checkPlayerPosition();
}
void moveRight() {
	 z -= speed ;
	 rotatePlayer();

	printPosition();
	checkPlayerPosition();
}
void drawPlayer1() {
	glPushMatrix();
	// glColor3f(1.0f, 0.0f, 0.0f);  // Red color
	glTranslatef(0.2, 0.4, 0.6);
	glRotated(115, 0, 1, 0);


	// Scale factor to match the teapot size
	float scaleFactor = 0.08f;

	// Body (cube)
	glPushMatrix();
	glScalef(scaleFactor * 0.5f, scaleFactor * 0.75f, scaleFactor * 0.25f); // Adjusted for proportion
	glutSolidCube(1.0);
	glPopMatrix();

	// Head (sphere)
	glPushMatrix();
	glTranslatef(0.0f, scaleFactor, 0.0f);  // Move up slightly for the head position
	glScalef(scaleFactor * 0.6f, scaleFactor * 0.6f, scaleFactor * 0.6f); // Adjusted for head size
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();

	// Left Arm
	glPushMatrix();
	glTranslatef(-scaleFactor * 0.4f, 0.0f, 0.0f);
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Arm
	glPushMatrix();
	glTranslatef(scaleFactor * 0.4f, 0.0f, 0.0f);
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Left Leg
	glPushMatrix();
	glTranslatef(-scaleFactor * 0.2f, -scaleFactor * 0.75f, 0.0f); // Position leg below the body
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Leg
	glPushMatrix();
	glTranslatef(scaleFactor * 0.2f, -scaleFactor * 0.75f, 0.0f); // Position leg below the body
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}
void draw() {
	// Code to draw the player at position (x, y, z) facing `angleY`
	//glPushMatrix();
	//glTranslatef(x, y, z);
	//// Draw the player model (e.g., a cube or teapot)
	//drawPlayer1();
	//glPopMatrix();
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(currentAngle, 0, 1, 0); // Rotate around Y-axis
	drawPlayer1(); // Your function to render the player
	glPopMatrix();
}
void animatePlayer(int value) {
	// Update rotation for continuous spinning
	playerRotation += 2.0f;
	if (playerRotation > 360.0f) playerRotation -= 360.0f;

	// Update arm angles for swinging
	if (leftArmUp) {
		leftArmAngle += 1.0f;
		if (leftArmAngle >= 20.0f) leftArmUp = false;
	}
	else {
		leftArmAngle -= 1.0f;
		if (leftArmAngle <= -20.0f) leftArmUp = true;
	}

	if (rightArmUp) {
		rightArmAngle += 1.0f;
		if (rightArmAngle >= 20.0f) rightArmUp = false;
	}
	else {
		rightArmAngle -= 1.0f;
		if (rightArmAngle <= -20.0f) rightArmUp = true;
	}

	glutPostRedisplay(); // Redraw the scene with updated values
	glutTimerFunc(16, animatePlayer, 0); // Schedule the next animation update (~60 FPS)
}
void drawPlayer2() {
	glPushMatrix();
	glTranslatef(playerX, 0.4, playerZ); // Use global variables for X and Z positions
	glRotatef(playerRotation, 0, 1, 0); // Rotate the entire player

	// Scale factor to match the teapot size
	float scaleFactor = 0.08f;

	// Body (cube)
	glPushMatrix();
	glScalef(scaleFactor * 0.5f, scaleFactor * 0.75f, scaleFactor * 0.25f); // Adjusted for proportion
	glutSolidCube(1.0);
	glPopMatrix();

	// Head (sphere)
	glPushMatrix();
	glTranslatef(0.0f, scaleFactor, 0.0f);  // Move up slightly for the head position
	glScalef(scaleFactor * 0.6f, scaleFactor * 0.6f, scaleFactor * 0.6f); // Adjusted for head size
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();

	// Left Arm (swinging)
	glPushMatrix();
	glTranslatef(-scaleFactor * 0.4f, 0.0f, 0.0f);
	glRotatef(leftArmAngle, 1.0f, 0.0f, 0.0f); // Swing animation
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Arm (swinging)
	glPushMatrix();
	glTranslatef(scaleFactor * 0.4f, 0.0f, 0.0f);
	glRotatef(rightArmAngle, 1.0f, 0.0f, 0.0f); // Swing animation
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Left Leg
	glPushMatrix();
	glTranslatef(-scaleFactor * 0.2f, -scaleFactor * 0.75f, 0.0f); // Position leg below the body
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Leg
	glPushMatrix();
	glTranslatef(scaleFactor * 0.2f, -scaleFactor * 0.75f, 0.0f); // Position leg below the body
	glScalef(scaleFactor * 0.25f, scaleFactor * 0.5f, scaleFactor * 0.25f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}
void drawTable(double topWid, double topThick, double legThick, double legLen){
	glPushMatrix();
	glTranslated(0, 0.3, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}
void drawRopeSegment(double x1, double y1, double z1, double x2, double y2, double z2) {
	glBegin(GL_LINES);
	glVertex3d(x1, y1, z1);
	glVertex3d(x2, y2, z2);
	glEnd();
}
void drawRope() {
	glLineWidth(3.0);  // Make the lines thicker
	//glColor3f(1.0, 0.0, 0.0); // Set line color to red

	double topWid = 0.6;
	double legThick = 0.02;
	double y1 = 0.3; // Y position of the table top
	double ropeHeights[3] = { y1 + 0.05, y1 + 0.09, y1 + 0.13 };

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;

	for (int i = 0; i < 3; i++) {
		double y = ropeHeights[i];

		// Draw ropes around the perimeter
		drawRopeSegment(dist, y, dist, -dist, y, dist); 
		drawRopeSegment(-dist, y, dist, -dist, y, -dist);
		drawRopeSegment(-dist, y, -dist, dist, y, -dist);
		drawRopeSegment(dist, y, -dist, dist, y, dist);
	}
}
// Helper function to render text
void renderText(float x, float y, void* font, const char* text) {
	glRasterPos2f(x, y); // Set position
	for (const char* c = text; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c); // Render each character
	}
}

// Placeholder function for confetti or other decorations
void renderConfetti() {
	// Example: Draw colorful particles
	glBegin(GL_POINTS);
	for (int i = 0; i < 100; i++) {
		glColor3f((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
		glVertex2f(rand() % 800, rand() % 600); // Random positions within the screen
	}
	glEnd();
}
void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}
void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	time_t currentTime = time(NULL);
	double elapsedSeconds = difftime(currentTime, startTime);
	if (elapsedSeconds >= 1000) {
		isGameLose = true;
	}
	if (isGameWin) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

		// Set up a 2D orthographic projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 800, 0, 600); // Set a 2D coordinate system (example dimensions)

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Set background color (optional)
		glClearColor(0.1f, 0.1f, 0.3f, 1.0f); // Dark blue background
		glClear(GL_COLOR_BUFFER_BIT);

		// Render "GAME WIN" text
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow text
		renderText(350, 300, GLUT_BITMAP_HELVETICA_18, "GAME WIN"); // Example coordinates

		// Add optional decorations (e.g., animations, particles)
		renderConfetti(); // Placeholder for custom decorations

		glFlush(); 
	}
	else if (isGameLose) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

		// Set up a 2D orthographic projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 800, 0, 600); // Set a 2D coordinate system (example dimensions)

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Set background color (optional)
		glClearColor(0.1f, 0.1f, 0.3f, 1.0f); // Dark blue background
		glClear(GL_COLOR_BUFFER_BIT);

		// Render "GAME WIN" text
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow text
		renderText(350, 300, GLUT_BITMAP_HELVETICA_18, "GAME LOSE"); // Example coordinates

		glFlush();
	}


	//glColor3f(0.0f, 0.0f, 0.0f); // Set color to black
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.4f); // Skin tone in floating-point (RGB)
	draw();
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.4f); // Skin tone in floating-point (RGB)
	drawPlayer2();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.8f, chairPosY, 0.3f);
	//glRotatef(chairAngle, 0.0f, 1.0f, 0.0f);
	glScalef(chairScale, chairScale, chairScale);
	glColor3f(1.0f, 1.0f, 0.0f); // Yellow
	drawTable(0.1f);
	glPopMatrix();

	//glPushMatrix();
	//glTranslatef(0.8f, chairPosY, 0.3f);
	//glRotatef(chairAngle, 0.0f, 1.0f, 0.0f);
	//glScalef(chairScale, chairScale, chairScale);
	//glColor3f(1.0f, 1.0f, 0.0f); // Yellow
	//drawChair(0.1f);
	//glPopMatrix();

	// Bottle
	glPushMatrix();
	glTranslatef(bottlePosX, 0.2f, 0.6f);
	glColor3f(bottleColor[0], bottleColor[1], bottleColor[2]);
	drawBottle(0.05f);
	glPopMatrix();

	// Fan
	glPushMatrix();
	glTranslatef(0.8f, 0.2f, 0.8f);
	glRotatef(fanAngle, 0.0f, 1.0f, 0.0f);
	glScalef(fanScale, fanScale, fanScale);
	glColor3f(1.0f, 0.647f, 0.0f); // Orange
	drawFan(0.15f);
	glPopMatrix();

	// Dumbbell
	glPushMatrix();
	glTranslatef(0.6f, 0.23f, dumbbellPosZ);
	glRotatef(dumbbellAngle, 1.0f, 0.0f, 0.0f);
	glColor3f(0.6f, 0.3f, 0.1f); // Brown
	drawDumbbell(0.05f);
	glPopMatrix();

	// Punching Bag
	glPushMatrix();
	glTranslatef(0.4f, 0.25f, 0.8f);
	glColor3f(bagColor[0], bagColor[1], bagColor[2]); // Dynamic color
	drawPunchingBag(0.1f);
	glPopMatrix();

	glColor3f(0.5f, 0.5f, 0.5f); // Grey in floating-point (RGB)

	glPushMatrix();
	glTranslated(0.4, 0, 0.4);
	drawRope();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.4, 0.0, 0.4);
	glColor3f(1.0f, 1.0f, 1.0f); // White in floating-point (RGB)
	drawTable(0.6, 0.02, 0.02, 0.5);
	glPopMatrix();
	glPushMatrix();
	//glColor3f(0.0f, 1.0f, 0.0f); // Green in floating-point (RGB)
	drawWall(0.2);
	glPopMatrix();
	//glColor3f(0.5f, 0.5f, 0.5f); // Grey in floating-point (RGB)
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1.0, 0, 0);
	glTranslated(0, -1, 0.0);
	drawWall(0.02);
	glPopMatrix();

	glFlush();
}
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 't':
		camera.topView();
		break;
	case 'f':
		camera.frontView();
		break;
	case 'g':
		camera.sideView();
		break;
	case 'i': // Move forward
		moveForward();
		playerDirection = 1;
		break;
	case 'k': // Move backward
		moveBackward();
		playerDirection = 2;
		break;
	case 'l': // Rotate left
		moveLeft();
		playerDirection = 3;
		break;
	case 'j': // Rotate right
		moveRight();
		playerDirection = 4;
		break;
	case 'z':
		isAnimating = !isAnimating;
		if (isAnimating) {
			animate(0);
		}
		break;
	case 'o':
		camera.moveZ(d);
		break;
	case 'p':
		camera.moveZ(-d);
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}
void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1000, 640);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Lab 6");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Applies color to the front faces

	glShadeModel(GL_SMOOTH);
	glutTimerFunc(5000, wallColorChange, 0);
	glutTimerFunc(0, animatePlayer, 0);


	glutMainLoop();
}
