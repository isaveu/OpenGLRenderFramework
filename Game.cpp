#include "Core.h" //Always at the top

#include "Game.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "BasicShader.h"
#include "PhongShader.h"
#include "Material.h"

/*
Features:
- SDL for window handling and input
- Core classes for common 3D objects
- Texture and OBJ model loader (wip)
- Shader support and basic camera/lighting
- Directional light, point light, specular light and specular map support
- Some example OBJs and textures included.


General TODOs:
- Wrap major classes in core header (wip)
- Component/GameObject system (wip)
- General code cleanup and framework improvements
- Improving rendering materials: cubemap, normal map, Skybox
- Shadows
- Geometry shader
- Improved texture IDs and smoothing group-based normals
- Transition to PBR

www.nickvanheer.com 
nickvanheer @ live.be
*/

Game::Game()
{
	mesh = new Mesh();
	meshFloor = new Mesh();
	shader = new PhongShader();
	shaderFloor = new PhongShader();

	camera = new Camera();
	camera->Move(Vector3(0, 0, -1), 2);


	//Torus transform
	transform = Transform();
	transform.SetProjection(Main::WIDTH, Main::HEIGHT, 60, 1000, 0.1);
	transform.SetCamera(camera);

	//floor
	transformFloor = Transform();
	transformFloor.SetProjection(Main::WIDTH, Main::HEIGHT, 60, 1000, 0.1);
	transformFloor.SetCamera(camera);

	//lighting setup
	Vector3 baseColor = Vector3(1, 1, 1);
	Vector3 lightDirection = Vector3(0, 0, -1);
	float lightIntensity = 3.0;

	Vector3 pLight1Position = Vector3(0, 0, 3);
	PointLight pLight1 = PointLight(BaseLight(Vector3(1, 0, 0), 10), Attenuation(0, 0, 1), pLight1Position, 6);
	std::vector<PointLight> points = { pLight1 };
	
	PhongShader::SetPointLights(points);
	PhongShader::AmbientLight = Vector3(0.3f, 0.3f, 0.3f);
	PhongShader::LightDirectional = DirectionalLight(BaseLight(baseColor, lightIntensity), lightDirection);

	//torus model TODO: flip in code flag
	mesh = ResourceLoader::LoadModel("resources/models/torusflipped.obj");

	//torus material
	material = new Material();
	material->Color = Vector3(0.2, 0.2, 0.2);
	material->SpecularPower = 50;
	material->SpecularIntensity = 10;
	material->SetTexture(ResourceLoader::LoadTexture("resources/textures/tiles.png"));
	material->SetSpecularMap(ResourceLoader::LoadTexture("resources/textures/tiles.png"));

	//floor
	meshFloor = ResourceLoader::LoadModel("resources/models/bigplane.obj");

	materialFloor = new Material();
	materialFloor->Color = Vector3(0.3, 0.3, 0.3);
	materialFloor->SpecularPower = 70;
	materialFloor->SpecularIntensity = 6;
	materialFloor->SetTexture(ResourceLoader::LoadTexture("resources/textures/TrainFloor.png"));
	materialFloor->SetSpecularMap(ResourceLoader::LoadTexture("resources/textures/TrainFloor_Spec.png"));

	RenderUtil::EnableTextures(true);
}


Game::~Game()
{
	Cleanup();
}


void Game::Start()
{
	Update();
}

void Game::Stop()
{

}

void Game::Input()
{
	float movAmount = Time::getDelta() * 10;
	float rotAmount = Time::getDelta() * 400;

	if (inputManager->IsKeyDown(SDL_SCANCODE_W))
		camera->Move(camera->GetForward(), movAmount);
	if (inputManager->IsKeyDown(SDL_SCANCODE_S))
		camera->Move(camera->GetForward(), -movAmount);
	if (inputManager->IsKeyDown(SDL_SCANCODE_A))
		camera->Move(camera->GetLeft(), movAmount);
	if (inputManager->IsKeyDown(SDL_SCANCODE_D))
		camera->Move(camera->GetRight(), movAmount);


	if (inputManager->IsKeyDown(SDL_SCANCODE_LEFT))
		camera->RotateY(-rotAmount);
	if (inputManager->IsKeyDown(SDL_SCANCODE_RIGHT))
		camera->RotateY(rotAmount);
	
}

float temp = 0.0f;
void Game::Update()
{
	temp += (float)Time::getDelta();
	transform.SetTranslation(4, 0, 2);
	transform.SetRotation(sin(temp) * 180, 0, 0);


	transformFloor.SetTranslation(0, 0, 5);
	transformFloor.SetRotation(-90 + sin(temp) * 20, 0, 0);

	transform.SetScale(1.0, 1.0, 1.0);
}

void Game::Render()
{
	//Torus
	shader->Bind(); //use shader program
	shader->SetMaterial(material); //set material
	shader->UpdateUniforms(transform.GetTransformation(), transform.GetProjectedTransformation()); //update shader variables and its material variables

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);
	mesh->Draw(); //program is unbound in the above draw method


	//Floor
	shaderFloor->SetMaterial(materialFloor); //set material
	shaderFloor->UpdateUniforms(transformFloor.GetTransformation(), transformFloor.GetProjectedTransformation()); //update shader variables and its material variables

	meshFloor->Draw();

}

void Game::Cleanup()
{
	delete mesh;
	delete meshFloor;
	delete shader; //deletes texture and material
	delete camera;
	delete inputManager;
}

void Game::SetInputManager(InputManager * inputManager)
{
	this->inputManager = inputManager;
}
