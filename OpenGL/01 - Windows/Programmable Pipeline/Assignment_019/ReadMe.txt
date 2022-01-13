//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Three Moving Lights on Sphere
------------------------------------------------------------------------------------------------------
Assignment Number 		: 017
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_017_Three_Rotating_Lights_Sphere
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Three_Rotating_Lights_Sphere.cpp, Headers.h, 
						vmath.h, Sphere.h, Sphere.lib, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. 
				GLuint VKeyIsPressed = 0; //Per-Vertex Lightling Toggle
				GLuint FKeyIsPressed = 0; //Per-Fragment Lightling Toggle

				//----------------------------------------------//
				//PER-VERTEX LIGHTING 
				GLuint gVertexShaderObject_PV;
				GLuint gFragmentShaderObject_PV;
				GLuint gShaderProgramObject_PV;

				//MATRIX UNIFORMS
				GLuint gModelMatrixUniform_PV;
				GLuint gViewMatrixUniform_PV;
				GLuint gProjectionMatrixUniform_PV;

				//LIGHT UNIFORMS
				GLuint LaUniform_Red_PV; //ambient light component
				GLuint LdUniform_Red_PV; //diffuse light component
				GLuint LsUniform_Red_PV; //specular light component
				GLuint lightPositionUniform_Red_PV; //light position

				GLuint LaUniform_Green_PV; //ambient light component
				GLuint LdUniform_Green_PV; //diffuse light component
				GLuint LsUniform_Green_PV; //specular light component
				GLuint lightPositionUniform_Green_PV; //light position

				GLuint LaUniform_Blue_PV; //ambient light component
				GLuint LdUniform_Blue_PV; //diffuse light component
				GLuint LsUniform_Blue_PV; //specular light component
				GLuint lightPositionUniform_Blue_PV; //light position

				//MATERIAL UNIFORMS
				GLuint KaUniform_PV; //ambient material component
				GLuint KdUniform_PV; //diffuse material component
				GLuint KsUniform_PV; //specular material component
				GLuint materialShininessUniform_PV; //shininess material

				GLuint LKeyPressedUniform_PV; //Light toggle

				//----------------------------------------------//
				//PER-FRAGMENT LIGHTING
				GLuint gVertexShaderObject_PF;
				GLuint gFragmentShaderObject_PF;
				GLuint gShaderProgramObject_PF;

				//MATRIX UNIFORMS
				GLuint gModelMatrixUniform_PF;
				GLuint gViewMatrixUniform_PF;
				GLuint gProjectionMatrixUniform_PF;

				//LIGHT UNIFORMS
				GLuint LaUniform_Red_PF; //ambient light component
				GLuint LdUniform_Red_PF; //diffuse light component
				GLuint LsUniform_Red_PF; //specular light component
				GLuint lightPositionUniform_Red_PF; //light position

				GLuint LaUniform_Green_PF; //ambient light component
				GLuint LdUniform_Green_PF; //diffuse light component
				GLuint LsUniform_Green_PF; //specular light component
				GLuint lightPositionUniform_Green_PF; //light position

				GLuint LaUniform_Blue_PF; //ambient light component
				GLuint LdUniform_Blue_PF; //diffuse light component
				GLuint LsUniform_Blue_PF; //specular light component
				GLuint lightPositionUniform_Blue_PF; //light position

				//MATERIAL UNIFORMS
				GLuint KaUniform_PF; //ambient material component
				GLuint KdUniform_PF; //diffuse material component
				GLuint KsUniform_PF; //specular material component
				GLuint materialShininessUniform_PF; //shininess material

				GLuint LKeyPressedUniform_PF; //Light toggle

				//----------------------------------------------//

				//light and material properties
				GLfloat lightAmbient_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightDiffuse_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f }; //RED
				GLfloat lightSpecular_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightPosition_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };

				GLfloat lightAmbient_Green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightDiffuse_Green[] = { 0.0f, 1.0f, 0.0f, 1.0f }; //BLUE
				GLfloat lightSpecular_Green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
				GLfloat lightPosition_Green[] = { 0.0f, 0.0f, 0.0f, 1.0f };

				GLfloat lightAmbient_Blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightDiffuse_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f }; //GREEN
				GLfloat lightSpecular_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
				GLfloat lightPosition_Blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };

				GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat materialShininess = 128.0f;


2. WinMain  : NO CHANGE

 
3. WndProc  : Changes are as follows:

				case WM_CHAR:
				switch (wParam)
				{
				case 'Q': //QUIT
				case 'q':
					DestroyWindow(hwnd);
					break;

				case 'F' : //PER-FRAGMENT SHADING TOGGLE
				case 'f': 
					if (FKeyIsPressed == 0)
					{
						FKeyIsPressed = 1;
						VKeyIsPressed = 0;
					}
					else if (FKeyIsPressed == 1)
					{
						FKeyIsPressed = 0;
					}
					break;

				case 'V': //PER-VERTEX SHADING TOGGLE
				case 'v':
					if (VKeyIsPressed == 0)
					{
						VKeyIsPressed = 1;
						FKeyIsPressed = 0;
					}
					else if (VKeyIsPressed == 1)
					{
						VKeyIsPressed = 0;
					}
					break;

				case 'L':
				case 'l':
					if (gbLights == false)
					{
						gbLights = true;
						VKeyIsPressed = 1; //PER-VERTEX SHADING WILL BE ENABLED BY DEFAULT
						FKeyIsPressed = 0;

					}
					else if (gbLights == true)
					{
						gbLights = false;
					}
					break;

				default:
					break;
				}
				break;

4. Initialize  : Changes are as follows:
			
			1. 	// PER VERTEX SHADER

				void getVertexShader_PV(void)
				{
					//code
					//code
					//VERTEX SHADER FOR PER-VERTEX LIGHTING
					//create shader
					gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

					//provide source code to shader
					//use 'core' profile i.e PROGRAMMABLE PIPELINE
				const GLchar* vertexShaderSourceCode_PV =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;" \
					"in vec3 vNormal;" \
					"uniform mat4 u_model_matrix;" \
					"uniform mat4 u_view_matrix;" \
					"uniform mat4 u_projection_matrix;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec3 u_LA_Red_PV;" \
					"uniform vec3 u_LD_Red_PV;" \
					"uniform vec3 u_LS_Red_PV;" \
					"uniform vec4 u_light_position_Red_PV;" \
					"uniform vec3 u_LA_Green_PV;" \
					"uniform vec3 u_LD_Green_PV;" \
					"uniform vec3 u_LS_Green_PV;" \
					"uniform vec4 u_light_position_Green_PV;" \
					"uniform vec3 u_LA_Blue_PV;" \
					"uniform vec3 u_LD_Blue_PV;" \
					"uniform vec3 u_LS_Blue_PV;" \
					"uniform vec4 u_light_position_Blue_PV;" \
					"uniform vec3 u_KA_PV;" \
					"uniform vec3 u_KD_PV;" \
					"uniform vec3 u_KS_PV;" \
					"uniform float u_material_shininess_PV;" \
					"out vec3 phong_ads_light_PV;" \
					"void main(void)" \
					"{" \
					"if (u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec3 ambient_Red_PV;" \
					"vec3 diffuse_Red_PV;" \
					"vec3 specular_Red_PV;" \
					"vec3 light_direction_Red;" \
					"vec3 reflection_vector_Red;" \

					"vec3 ambient_Green_PV;" \
					"vec3 diffuse_Green_PV;" \
					"vec3 specular_Green_PV;" \
					"vec3 light_direction_Green;" \
					"vec3 reflection_vector_Green;" \

					"vec3 ambient_Blue_PV;" \
					"vec3 diffuse_Blue_PV;" \
					"vec3 specular_Blue_PV;" \
					"vec3 light_direction_Blue;" \
					"vec3 reflection_vector_Blue;" \

					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
					"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
					"vec3 view_vector = normalize(-eye_coordinates.xyz);" \

					"light_direction_Red = normalize(vec3(u_light_position_Red_PV - eye_coordinates));" \
					"reflection_vector_Red = reflect(-light_direction_Red, transformed_normal);" \
					"ambient_Red_PV = u_LA_Red_PV * u_KA_PV;" \
					"diffuse_Red_PV = u_LD_Red_PV * u_KD_PV * max(dot(light_direction_Red, transformed_normal), 0.0f);" \
					"specular_Red_PV = u_LS_Red_PV * u_KS_PV * pow(max(dot(reflection_vector_Red, view_vector), 0.0f), u_material_shininess_PV);" \

					"light_direction_Green = normalize(vec3(u_light_position_Green_PV - eye_coordinates));" \
					"reflection_vector_Green = reflect(-light_direction_Green, transformed_normal);" \
					"ambient_Green_PV = u_LA_Green_PV * u_KA_PV;" \
					"diffuse_Green_PV = u_LD_Green_PV * u_KD_PV * max(dot(light_direction_Green, transformed_normal), 0.0f);" \
					"specular_Green_PV = u_LS_Green_PV * u_KS_PV * pow(max(dot(reflection_vector_Green, view_vector), 0.0f), u_material_shininess_PV);" \

					"light_direction_Blue = normalize(vec3(u_light_position_Blue_PV - eye_coordinates));" \
					"reflection_vector_Blue = reflect(-light_direction_Blue, transformed_normal);" \
					"ambient_Blue_PV = u_LA_Blue_PV * u_KA_PV;" \
					"diffuse_Blue_PV = u_LD_Blue_PV * u_KD_PV * max(dot(light_direction_Blue, transformed_normal), 0.0f);" \
					"specular_Blue_PV = u_LS_Blue_PV * u_KS_PV * pow(max(dot(reflection_vector_Blue, view_vector), 0.0f), u_material_shininess_PV);" \

					"phong_ads_light_PV = ambient_Red_PV + diffuse_Red_PV + specular_Red_PV" \
					" + ambient_Green_PV + diffuse_Green_PV + specular_Green_PV" \
					" + ambient_Blue_PV + diffuse_Blue_PV + specular_Blue_PV;" \
					//"phong_ads_light_PV = phong_ads_light_PV + ambient_Green_PV + diffuse_Green_PV + specular_Green_PV;" \
					//"phong_ads_light_PV = phong_ads_light_PV + ambient_Blue_PV + diffuse_Blue_PV + specular_Blue_PV;" \
					
					"}" \
					"else" \
					"{" \
					"phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);" \
					"}" \
					"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
					"}";
				}

				void getFragmentShader_PV(void)
				{
					//code
					//FRAGMENT SHADER FOR PER-VERTEX LIGHTING
					//create shader
					gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

					//provide source code to shader
					const GLchar* fragmentShaderSourceCode_PV =
						"#version 450 core" \
						"\n" \
						"in vec3 phong_ads_light_PV;" \
						"out vec4 FragColor;" \
						"void main(void)" \
						"{" \
						"FragColor = vec4(phong_ads_light_PV, 1.0f);" \
						"}";

				}


			2. //PER FRAGMENT SHADER
				
				void getVertexShader_PF(void)
				{
					//code
					//VERTEX SHADER
					//create shader
					gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

					//provide source code to shader
					//use 'core' profile i.e PROGRAMMABLE PIPELINE
				const GLchar* vertexShaderSourceCode_PF =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;" \
					"in vec3 vNormal;" \
					"uniform mat4 u_model_matrix;" \
					"uniform mat4 u_view_matrix;" \
					"uniform mat4 u_projection_matrix;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec4 u_light_position_Red_PF;" \
					"uniform vec4 u_light_position_Green_PF;" \
					"uniform vec4 u_light_position_Blue_PF;" \
					"out vec3 transformed_normal;" \
					"out vec3 light_direction_Red;" \
					"out vec3 light_direction_Green;" \
					"out vec3 light_direction_Blue;" \
					"out vec3 view_vector;" \
					"void main(void)" \
					"{" \
					"if(u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
					"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
					"light_direction_Red = vec3(u_light_position_Red_PF - eye_coordinates);" \
					"light_direction_Green = vec3(u_light_position_Green_PF - eye_coordinates);" \
					"light_direction_Blue = vec3(u_light_position_Blue_PF - eye_coordinates);" \
					"view_vector = -eye_coordinates.xyz;" \
					"}" \
					"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
					"}";

					
				}

				void getFragmentShader_PF(void) 
				{
					//code
					//FRAGMENT SHADER
					//create shader
					gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

					//provide source code to shader
				const GLchar* fragmentShaderSourceCode_PF =
					"#version 450 core" \
					"\n" \
					"in vec3 transformed_normal;" \
					"in vec3 light_direction_Red;" \
					"in vec3 light_direction_Green;" \
					"in vec3 light_direction_Blue;" \
					"in vec3 view_vector;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec3 u_LA_Red_PF;" \
					"uniform vec3 u_LD_Red_PF;" \
					"uniform vec3 u_LS_Red_PF;" \
					"uniform vec3 u_LA_Green_PF;" \
					"uniform vec3 u_LD_Green_PF;" \
					"uniform vec3 u_LS_Green_PF;" \
					"uniform vec3 u_LA_Blue_PF;" \
					"uniform vec3 u_LD_Blue_PF;" \
					"uniform vec3 u_LS_Blue_PF;" \
					"uniform vec3 u_KA_PF;" \
					"uniform vec3 u_KD_PF;" \
					"uniform vec3 u_KS_PF;" \
					"uniform float u_material_shininess_PF;" \
					"out vec4 FragColor;" \
					"void main(void)" \
					"{" \
					"vec3 phong_ads_light_PF;" \
					"if(u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
					"vec3 normalized_light_direction_Red = normalize(light_direction_Red);" \
					"vec3 normalized_light_direction_Green = normalize(light_direction_Green);" \
					"vec3 normalized_light_direction_Blue = normalize(light_direction_Blue);" \
					"vec3 normalized_view_vector = normalize(view_vector);" \
					
					"vec3 ambient_Red_PF = u_LA_Red_PF * u_KA_PF;" \
					"vec3 diffuse_Red_PF = u_LD_Red_PF * u_KD_PF * max(dot(normalized_light_direction_Red, normalized_transformed_normal), 0.0f);" \
					"vec3 reflection_vector_Red = reflect(-normalized_light_direction_Red, normalized_transformed_normal);" \
					"vec3 specular_Red_PF = u_LS_Red_PF * u_KS_PF * pow(max(dot(reflection_vector_Red, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

					"vec3 ambient_Green_PF = u_LA_Green_PF * u_KA_PF;" \
					"vec3 diffuse_Green_PF = u_LD_Green_PF * u_KD_PF * max(dot(normalized_light_direction_Green, normalized_transformed_normal), 0.0f);" \
					"vec3 reflection_vector_Green = reflect(-normalized_light_direction_Green, normalized_transformed_normal);" \
					"vec3 specular_Green_PF = u_LS_Green_PF * u_KS_PF * pow(max(dot(reflection_vector_Green, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

					"vec3 ambient_Blue_PF = u_LA_Blue_PF * u_KA_PF;" \
					"vec3 diffuse_Blue_PF = u_LD_Blue_PF * u_KD_PF * max(dot(normalized_light_direction_Blue, normalized_transformed_normal), 0.0f);" \
					"vec3 reflection_vector_Blue = reflect(-normalized_light_direction_Blue, normalized_transformed_normal);" \
					"vec3 specular_Blue_PF = u_LS_Blue_PF * u_KS_PF * pow(max(dot(reflection_vector_Blue, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

					"phong_ads_light_PF = ambient_Red_PF + diffuse_Red_PF + specular_Red_PF;" \
					"phong_ads_light_PF = phong_ads_light_PF + ambient_Green_PF + diffuse_Green_PF + specular_Green_PF;" \
					"phong_ads_light_PF = phong_ads_light_PF + ambient_Blue_PF + diffuse_Blue_PF + specular_Blue_PF;" \
					"}" \
					"else" \
					"{" \
					"phong_ads_light_PF = vec3(1.0f,1.0f,1.0f);" \
					"}" \
					"FragColor = vec4(phong_ads_light_PF, 1.0f);" \
					"}";

				}

				
			3. //get Uniform Location
				gModelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
				gViewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
				gProjectionMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");

				LKeyPressedUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_L_KEY_PRESSED");

				LaUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Red_PV");
				LdUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Red_PV");
				LsUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Red_PV");
				lightPositionUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Red_PV");

				LaUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Green_PV");
				LdUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Green_PV");
				LsUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Green_PV");
				lightPositionUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Green_PV");

				LaUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Blue_PV");
				LdUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Blue_PV");
				LsUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Blue_PV");
				lightPositionUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Blue_PV");

				KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KA_PV");
				KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KD_PV");
				KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KS_PV");
				materialShininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_material_shininess_PV");

				/////////////////////////////////////////////////

				//get Uniform Location
				gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
				gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
				gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");

				LKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_L_KEY_PRESSED");

				LaUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Red_PF");
				LdUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Red_PF");
				LsUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Red_PF");
				lightPositionUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Red_PF");

				LaUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Green_PF");
				LdUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Green_PF");
				LsUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Green_PF");
				lightPositionUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Green_PF");

				LaUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Blue_PF");
				LdUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Blue_PF");
				LsUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Blue_PF");
				lightPositionUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Blue_PF");

				KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KA_PF");
				KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KD_PF");
				KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KS_PF");
				materialShininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_material_shininess_PF");

5. Display() - Chnages are as follows: 

			.
			.
			.
			glUseProgram(gShaderProgramObject_PV); //using Per-vertex by default

			//USING PER-VERTEX LIGHITNG
			if (VKeyIsPressed == 1)
			{
				if (gbLights == true)
				{
					lightPosition_Red[0] = 0.0f; //x
					lightPosition_Red[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
					lightPosition_Red[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
					lightPosition_Red[3] = 1.0f; //w

					lightPosition_Green[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
					lightPosition_Green[1] = 0.0f; //y
					lightPosition_Green[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
					lightPosition_Green[3] = 1.0f; //w

					lightPosition_Blue[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
					lightPosition_Blue[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
					lightPosition_Blue[2] = 0.0f; //z
					lightPosition_Blue[3] = 1.0f; //w

					glUniform1i(LKeyPressedUniform_PV, 1);
					glUniform3fv(LaUniform_Red_PV, 1, (GLfloat*)lightAmbient_Red);
					glUniform3fv(LdUniform_Red_PV, 1, (GLfloat*)lightDiffuse_Red);
					glUniform3fv(LsUniform_Red_PV, 1, (GLfloat*)lightSpecular_Red);
					glUniform4fv(lightPositionUniform_Red_PV, 1, (GLfloat*)lightPosition_Red);

					glUniform3fv(LaUniform_Green_PV, 1, (GLfloat*)lightAmbient_Green);
					glUniform3fv(LdUniform_Green_PV, 1, (GLfloat*)lightDiffuse_Green);
					glUniform3fv(LsUniform_Green_PV, 1, (GLfloat*)lightSpecular_Green);
					glUniform4fv(lightPositionUniform_Green_PV, 1, (GLfloat*)lightPosition_Green);

					glUniform3fv(LaUniform_Blue_PV, 1, (GLfloat*)lightAmbient_Blue);
					glUniform3fv(LdUniform_Blue_PV, 1, (GLfloat*)lightDiffuse_Blue);
					glUniform3fv(LsUniform_Blue_PV, 1, (GLfloat*)lightSpecular_Blue);
					glUniform4fv(lightPositionUniform_Blue_PV, 1, (GLfloat*)lightPosition_Blue);

					glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient);
					glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse);
					glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular);
					glUniform1f(materialShininessUniform_PV, materialShininess);

				}
				else
				{
					glUniform1i(LKeyPressedUniform_PV, 0);
				}

				glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
				glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
			}

			else if (FKeyIsPressed == 1)
			{
				glUseProgram(gShaderProgramObject_PF);

				if (gbLights == true)
				{
					lightPosition_Red[0] = 0.0f; //x
					lightPosition_Red[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
					lightPosition_Red[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
					lightPosition_Red[3] = 1.0f; //w

					lightPosition_Green[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
					lightPosition_Green[1] = 0.0f; //y
					lightPosition_Green[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
					lightPosition_Green[3] = 1.0f; //w

					lightPosition_Blue[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
					lightPosition_Blue[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
					lightPosition_Blue[2] = 0.0f; //z
					lightPosition_Blue[3] = 1.0f; //w

					glUniform1i(LKeyPressedUniform_PF, 1);
					glUniform3fv(LaUniform_Red_PF, 1, (GLfloat*)lightAmbient_Red);
					glUniform3fv(LdUniform_Red_PF, 1, (GLfloat*)lightDiffuse_Red);
					glUniform3fv(LsUniform_Red_PF, 1, (GLfloat*)lightSpecular_Red);
					glUniform4fv(lightPositionUniform_Red_PF, 1, (GLfloat*)lightPosition_Red);

					glUniform3fv(LaUniform_Green_PF, 1, (GLfloat*)lightAmbient_Green);
					glUniform3fv(LdUniform_Green_PF, 1, (GLfloat*)lightDiffuse_Green);
					glUniform3fv(LsUniform_Green_PF, 1, (GLfloat*)lightSpecular_Green);
					glUniform4fv(lightPositionUniform_Green_PF, 1, (GLfloat*)lightPosition_Green);

					glUniform3fv(LaUniform_Blue_PF, 1, (GLfloat*)lightAmbient_Blue);
					glUniform3fv(LdUniform_Blue_PF, 1, (GLfloat*)lightDiffuse_Blue);
					glUniform3fv(LsUniform_Blue_PF, 1, (GLfloat*)lightSpecular_Blue);
					glUniform4fv(lightPositionUniform_Blue_PF, 1, (GLfloat*)lightPosition_Blue);

					glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient);
					glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse);
					glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular);
					glUniform1f(materialShininessUniform_PF, materialShininess);

				}
				else
				{
					glUniform1i(LKeyPressedUniform_PF, 0);
				}

				glUniformMatrix4fv(gModelMatrixUniform_PF, 1, GL_FALSE, translationMatrix);
				glUniformMatrix4fv(gViewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);
			}

			else
			{
				glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
				glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
			}

			.
			.
			.

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Header.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
