//=============================================================================================
// Mintaprogram: Z�ld h�romsz�g. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : 
// Neptun : 
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

/*// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char * const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers

	uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
	layout(location = 0) in vec2 vtxPos;
	layout(location = 1) in vec2 vtxUV;
	out vec2 texcoord;
	void main() {
		gl_Position = vec4(vtxPos, 0, 1) * MVP;
		texcoord = vtxUV;
	}
)";

// fragment shader in GLSL
const char * const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers
	
	uniform sampler2D samplerUnit;
	in vec2 texcoord;
	out vec4 fragmentColor;
	void main() {
		fragmentColor = texture(samplerUnit, texcoord);
	}
)";*/

const char * const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers

	uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
	layout(location = 0) in vec3 vp;	// Varying input: vp = vertex position is expected in attrib array 0

	void main() {
		gl_Position = vec4(vp.x, vp.y, vp.z, 1) * MVP;		// transform vp from modeling space to normalized device space
	}
)";

const char * const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers

	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 outColor;		// computed color of the current pixel

	void main() {
		outColor = vec4(color, 1);	// computed color is the color of the primitive
	}
)";

GPUProgram gpuProgram; // vertex and fragment shaders

class Object {
protected:
	unsigned int vao, vbo;
	std::vector<vec2> points;
	virtual void calculatePoints() = 0;
	virtual void updateAndDraw() = 0;
public:
	void create() {
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

		calculatePoints();
	}

	void draw() {
		updateAndDraw();
		mat4 MVPTransform ( 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 );
		gpuProgram.setUniform(MVPTransform, "MVP");

		gpuProgram.setUniform(vec3(1.0f, 0.0f, 0.0f), "color");
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float) * 2, &points[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, points.size());
	}


};

class Line : public Object {
	float d, phi;
	void calculatePoints() override {
		points.clear();

		// Calculate the radius of the circle so that it crosses the big circle perpendicularly
		float r = (1.0f - d * d) / 2.0f / d;

		int a = - 100 * (phi / M_PIf / 2);

		bool visible = false;

		for (int i = a; i < 100 + a; i++) {
			float x = cosf((float) (i / 100.0f * 2 * M_PI)) * r + (d + r) * cosf(phi);
			float y = - sinf((float) (i / 100.0f * 2 * M_PI)) * r + (d + r) * sinf(phi);
			if (x * x + y * y < 1.0f) {
				if (!visible) {
					float xprev = cosf((float) ((i - 1) / 100.0f * 2 * M_PI)) * r + (d + r) * cosf(phi);
					float yprev = - sinf((float) ((i - 1) / 100.0f * 2 * M_PI)) * r + (d + r) * sinf(phi);
					points.push_back(vec2(xprev, yprev));
					visible = true;
				}
				points.push_back(vec2(x, y));
			}
			else if (visible) {
				points.push_back(vec2(x, y));
				visible = false;
			}
		}
		printf("%f %f\n", d, phi);
	}
	void updateAndDraw() override {}
public:
	Line(float d, float phi) {
		this->d = (- 2 + sqrtf(4 + 4 * sinhf(d) * sinhf(d))) / 2.0f / sinhf(d);
		this->phi = phi;
		calculatePoints();
	}

	bool isInside(float x, float y) {
		float r = (1.0f - d * d) / 2.0f / d;
		float x1 = (d + r) * cosf(phi);
		float y1 = (d + r) * sinf(phi);
		return (x - x1) * (x - x1) + (y - y1) * (y - y1) < r * r;
	}
};

class Poincare : public Object {
protected:
	std::vector<Line*> lines;

	void calculatePoints() override {
		points.clear();

		for (int i = 0; i < 100; i++) {
			float x = cosf((float) (i / 100.0f * 2 * M_PI));
			float y = - sinf((float) (i / 100.0f * 2 * M_PI));
			points.push_back(vec2(x, y));
		}
	}

	void updateAndDraw() override {
		for (Line* line : lines) {
			line->draw();
		}
	}

	bool isEvenDeep(float x, float y) {
		int n = 0;
		if (x * x + y * y < 1.0f) {
			n++;
		}
		for (Line* line : lines) {
			n += line->isInside(x, y) ? 1 : 0;
		}
		return n % 2 == 0;
	}
public:
	void addLine(Line *line) {
		lines.push_back(line);
	}
	void drawLines() {
		for (Line* line : lines) {
			line->draw();
		}
	}
	int RenderToTexture(int w, int h, int sampling = GL_LINEAR) {
		vec3 * image = new vec3[w * h];
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				image[i * h + j] = isEvenDeep((i - w / 2.0f) / w, (j - h / 2.0f) / h) ? vec3(0.0f, 0.0f, 0.0f) : vec3(0.0f, 1.0f, 1.0f);
			}
		}
		unsigned int textureId;
		glGenTextures(1, &textureId);  				// id generation
		glBindTexture(GL_TEXTURE_2D, textureId);    // binding

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, &image[0]); // To GPU
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling); // sampling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
		return textureId;
	}
};

class Star {
	Poincare poincare;
	unsigned int vao, vbo[2], textureId;
public:
	void create() {
		poincare.create();

		for (int i = 0; i < 9; i++) {
			float phi = i / 9.0f * 2 * M_PI;
			float n = 0.5f;
			Line* line = new Line(n, phi);
			line->create();
			poincare.addLine(line);
			for (int j = 0; j < 5; j++) {
				n += 1.0f;
				Line* line = new Line(n, phi);
				line->create();
				poincare.addLine(line);
			}
		}

		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));




	}
	void draw() {
		int sampler = 0; // which sampler unit should be used
		gpuProgram.setUniform(sampler, "samplerUnit");
		glActiveTexture(GL_TEXTURE0 + sampler); // = GL_TEXTURE0
		glBindTexture(GL_TEXTURE_2D, textureId);

	}
};

Poincare poincare;


// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

    poincare.create();

    for (int i = 0; i < 9; i++) {
        float phi = i / 9.0f * 2 * M_PI;
        float n = 0.5f;
        Line* line = new Line(n, phi);
        line->create();
        poincare.addLine(line);
        for (int j = 0; j < 5; j++) {
            n += 1.0f;
            Line* line = new Line(n, phi);
            line->create();
            poincare.addLine(line);
        }
    }

	// create program for the GPU
	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);     // background color
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	poincare.draw();

	glutSwapBuffers(); // exchange buffers for double buffering
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();         // if d, invalidate display, i.e. redraw
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) { // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	glutPostRedisplay();
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}
