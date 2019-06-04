#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

unsigned int vao, vbo;

struct shader {
	unsigned int type;
	const char *source;
	};

SDL_Window *window;
SDL_GLContext *context;

void
setup_sdl(char *title, int width, int height)
	{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	window = SDL_CreateWindow(title, 0, 0, width, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	if (!context) {
		puts("Error creating context.");
		exit(-1);
		}
	}

char *
filetobuf(char *file_name)
	{
	FILE *fp;
	unsigned int size;
	char *buffer;
	fp = fopen(file_name, "rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	size = (ftell(fp)) ;
	rewind(fp);
	buffer = malloc(size);
	fread(buffer, size, 1, fp);
	fclose(fp);
	buffer[size-1] = 0;

	return buffer;
	}

void
setup_geometry_buffer()
	{
	float vertices[] = { 	
		-.5,.5,0,	-.5,-.5,0,	.5,.5,0,
		.5,.5,0,	-.5,-.5,0,	.5,-.5,0,
		};

	/* Set up VAO*/
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Set up VBO */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*6, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	}

void
check_shader_error(unsigned int shader)
	{
	int IsCompiled;
	int maxLength;
	char *vertexInfoLog;
	vertexInfoLog = malloc(maxLength);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &IsCompiled);
    if(!IsCompiled)
    {
       glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

       /* The maxLength includes the NULL character */
       vertexInfoLog = (char *)malloc(maxLength);

       glGetShaderInfoLog(shader, maxLength, &maxLength, vertexInfoLog);

	puts(vertexInfoLog);
       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
       /* In this simple program, we'll just leave */
       free(vertexInfoLog);
       return;
    } ;
	}
void
check_program_error(unsigned int prog)
	{
	char *log;
	int maxLength = 256;
	int IsLinked;
	log = malloc(maxLength);
	 glGetProgramiv(prog, GL_LINK_STATUS, (int *)&IsLinked);
    if(!IsLinked)
    {
    	puts("Problem!!");
       /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
       glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength);

       /* The maxLength includes the NULL character */
       log = (char *)malloc(maxLength);

       /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
       glGetProgramInfoLog(prog, maxLength, &maxLength, log);
	
	puts(log);
       /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
       /* In this simple program, we'll just leave */
       free(log);
       return;
    }
	}

unsigned int
create_program(unsigned int count, struct shader *shaders)
	{
	unsigned int program;
	unsigned int shader_object;
	int i;
	program = glCreateProgram();

	for (i=0; i<count; i++)
		{
		if (!shaders[i].source) {
			puts("Invalid shader source.");
			exit(-1);
			}
		shader_object = glCreateShader(shaders[i].type);
		glShaderSource(shader_object, 1, &shaders[i].source, 0);
		glCompileShader(shader_object);
		check_shader_error(shader_object);
		glAttachShader(program, shader_object);
		}
	glLinkProgram(program);
	check_program_error(program);
	glUseProgram(program);

	return program;
	}

void
destroy_program(unsigned int count, struct shader *shaders)
	{
	int i;

	for (i=0; i<count; i++)
		free((char*) shaders[i].source);
	}

int
main(int argc, char *argv[])
	{
	int quit=0;
	unsigned int shader_program_object;
	SDL_Event event;

	puts("OpenGL 3.3 Test");

	setup_sdl("OpenGL 3.3 Test", 512, 512);
	glewInit();

	setup_geometry_buffer();

	/*Set up shader*/
	struct shader shaders[2];
	shaders[0].type = GL_VERTEX_SHADER;
	shaders[0].source = filetobuf("shader.vert");
	shaders[1].type = GL_FRAGMENT_SHADER;
	shaders[1].source = filetobuf("shader.frag");
	/* Compile sources into program */
	shader_program_object = create_program(2, shaders);

	puts((const char*)glGetString(GL_VERSION));

	/* Bind attrib to a variable in the shader */
	glBindAttribLocation(shader_program_object, 0, "in_Position");
	while (!quit) {
		SDL_PollEvent(&event);
		if (event.type == SDL_KEYDOWN) if (event.key.keysym.sym==SDLK_ESCAPE) quit=1;

		glDrawArrays(GL_TRIANGLES, 0, 6);
		SDL_GL_SwapWindow(window);
		}

	/* Tidy up allocated resources. */
	destroy_program(2, shaders);
	glDeleteProgram(shader_program_object);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
	}
