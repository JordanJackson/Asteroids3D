#include "GameObject.h"
;
Collider GameObject::GetCollider()
{
	return collider;
}

glm::vec3 GameObject::GetPosition()
{
	return position;
}

float GameObject::GetYaw()
{
	return yaw;
}

float GameObject::GetPitch()
{
	return pitch;
}

float GameObject::GetRoll()
{
	return roll;
}

glm::mat4 GameObject::GetRotationMatrix()
{
	return this->rotationMatrix;
}

glm::vec3 GameObject::GetScale()
{
	return scale;
}

glm::vec2 GameObject::GetVelocity()
{
	return velocity;
}

void GameObject::SetMesh(glsh::Mesh* mesh)
{
	this->mesh = mesh;
}

void GameObject::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void GameObject::SetYaw(float angle)
{
	this->yaw = angle;
	UpdateRotationMatrix();
}

void GameObject::SetPitch(float angle)
{
	this->pitch = angle;
	UpdateRotationMatrix();
}

void GameObject::SetRoll(float angle)
{
	this->roll = angle;
	UpdateRotationMatrix();
}

void GameObject::SetYawRotationSpeed(float rotationSpeed)
{
	this->yawRotationSpeed = rotationSpeed;
}

void GameObject::SetPitchRotationSpeed(float rotationSpeed)
{
	this->pitchRotationSpeed = rotationSpeed;
}

void GameObject::SetRollRotationSpeed(float rotationSpeed)
{
	this->rollRotationSpeed = rotationSpeed;
}

void GameObject::SetScale(glm::vec3 scale)
{
	this->scale = scale;
}

void GameObject::SetSpeed(float speed)
{
	this->speed = speed;

	velocity = glm::vec2(speed * cos(glm::radians(yaw)), speed * sin(glm::radians(yaw)));
}

void GameObject::SetVelocity(glm::vec2 velocity)
{
	this->velocity = velocity;
}

void GameObject::UpdatePosition(glm::vec3 delta, float fov, int w, int h)
{
	position += delta;

	// Resorted to "popping" wrap-around, couldn't get smooth wrap-around

	// window aspect ratio
	float aspectRatio = w / (float)h;

	// dimensions of viewable area
	float viewHeight = fov;
	float viewWidth = viewHeight * aspectRatio;

	// bounds of viewable area
	float viewLeft = -0.7f * viewWidth;
	float viewRight = viewLeft + viewWidth * 1.4f;
	float viewBottom = -1.0f * viewHeight;
	float viewTop = viewBottom + viewHeight;

	// wrap around
	if (position.x < viewLeft)
	{
		position.x = viewRight;
	}
	else if (position.x > viewRight)
	{
		position.x = viewLeft;
	}

	if (position.y < viewBottom)
	{
		
		position.y = viewHeight;

	}
	else if (position.y > viewHeight)
	{
		position.y = viewBottom;
	}
}

void GameObject::UpdateYaw(float angle)
{
	yaw += angle;
	UpdateRotationMatrix();
}

void GameObject::UpdatePitch(float angle)
{
	pitch += angle;
	UpdateRotationMatrix();
}

void GameObject::UpdateRoll(float angle)
{
	roll += angle;
	UpdateRotationMatrix();
}

void GameObject::UpdateRotationMatrix()
{
	glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::quat yawQuat = glsh::CreateQuaternion(glm::radians(yaw), yAxis);
	glm::quat pitchQuat = glsh::CreateQuaternion(glm::radians(pitch), xAxis);
	glm::quat rollQuat = glsh::CreateQuaternion(glm::radians(roll), zAxis);
	glm::quat Q = pitchQuat * yawQuat * rollQuat;
	rotationMatrix = glm::toMat4(Q);
}

void GameObject::UpdateScale(glm::vec3 scale)
{
	this->scale += scale;
}

void GameObject::UpdateVelocity(glm::vec2 velocity)
{
	this->velocity += velocity;
}

void GameObject::Render()
{
	mesh->draw();
}

bool GameObject::CheckCollision(GameObject* other)
{
	// use circle to circle collision checking
	if (glm::length(position - other->GetPosition()) <= (collider.radius + other->GetCollider().radius) * 1.2f)
	{
		return true;
	}
	else
	{
		return false;
	}
}