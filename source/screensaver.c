#include "raylib.h"
#include "raymath.h"
#define RL_MATRIX_TYPE
#include "rlgl.h"

Color cubeColor;
Vector3 cubePosition = {0,0,0};
Camera3D camera;

// Draw a grid centered at (0, 0, 0)
static void DrawGrid(int slices, float spacing)
{
	int halfSlices = slices / 2;

	rlBegin(RL_LINES);
	for (int i = -halfSlices; i <= halfSlices; i++)
	{
		if (i == 0)
		{
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
			rlColor3f(0.5f, 0.5f, 0.5f);
		}
		else
		{
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
			rlColor3f(0.75f, 0.75f, 0.75f);
		}

		rlVertex3f((float)i * spacing, 0.0f, (float)-halfSlices * spacing);
		rlVertex3f((float)i * spacing, 0.0f, (float)halfSlices * spacing);

		rlVertex3f((float)-halfSlices * spacing, 0.0f, (float)i * spacing);
		rlVertex3f((float)halfSlices * spacing, 0.0f, (float)i * spacing);
	}
	rlEnd();
}

// Draw cube
// NOTE: Cube position is the center position
static void DrawCube(Vector3 position, float width, float height, float length, Color color)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	rlPushMatrix();

	// NOTE: Be careful! Function order matters (rotate -> scale -> translate)
	rlTranslatef(position.x, position.y, position.z);
	//rlScalef(2.0f, 2.0f, 2.0f);
	//rlRotatef(45, 0, 1, 0);

	rlBegin(RL_TRIANGLES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front Face -----------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left

	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right

	// Back Face ------------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right

	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left

	// Top Face -------------------------------------------------------
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Bottom Right

	// Bottom Face ----------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left

	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left

	// Right face -----------------------------------------------------
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Left

	// Left Face ------------------------------------------------------
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Right

	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlEnd();
	rlPopMatrix();
}

// Draw cube wires
static void DrawCubeWires(Vector3 position, float width, float height, float length, Color color)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	rlPushMatrix();

	rlTranslatef(position.x, position.y, position.z);
	//rlRotatef(45, 0, 1, 0);

	rlBegin(RL_LINES);
	rlColor4ub(color.r, color.g, color.b, color.a);

	// Front Face -----------------------------------------------------
	// Bottom Line
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right

	// Left Line
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right

	// Top Line
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left

	// Right Line
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Bottom Left

	// Back Face ------------------------------------------------------
	// Bottom Line
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right

	// Left Line
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Bottom Right
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right

	// Top Line
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left

	// Right Line
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Bottom Left

	// Top Face -------------------------------------------------------
	// Left Line
	rlVertex3f(x - width / 2, y + height / 2, z + length / 2);  // Top Left Front
	rlVertex3f(x - width / 2, y + height / 2, z - length / 2);  // Top Left Back

	// Right Line
	rlVertex3f(x + width / 2, y + height / 2, z + length / 2);  // Top Right Front
	rlVertex3f(x + width / 2, y + height / 2, z - length / 2);  // Top Right Back

	// Bottom Face  ---------------------------------------------------
	// Left Line
	rlVertex3f(x - width / 2, y - height / 2, z + length / 2);  // Top Left Front
	rlVertex3f(x - width / 2, y - height / 2, z - length / 2);  // Top Left Back

	// Right Line
	rlVertex3f(x + width / 2, y - height / 2, z + length / 2);  // Top Right Front
	rlVertex3f(x + width / 2, y - height / 2, z - length / 2);  // Top Right Back
	rlEnd();
	rlPopMatrix();
}


void Init()
{
	camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
	cubeColor = RED;
}

void Update()
{
	cubeColor.r = rand();
	cubeColor.g = rand();
	cubeColor.b = rand();

	UpdateCamera(&camera, CAMERA_ORBITAL);

	//----------------------------------------------------------------------------------

	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginMode3D(camera);

	DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, cubeColor);
	DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

	DrawGrid(10, 1.0f);

	EndMode3D();
	EndDrawing();
	//-----------
}

void Destroy()
{
}

