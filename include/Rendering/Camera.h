/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Camera_h__
#define Camera_h__

namespace dd
{

class Camera
{
public:
	/** Camera constructor.
	
		@param aspectRatio Aspect ratio.
		@param yFOV Vertical FOV.
		@param nearClip Near clipping plane in meters.
		@param farClip Far clipping plane in meters.
	*/
	Camera(float aspectRatio, float yFOV, float nearClip, float farClip);

	/** Forward vector of the camera */
	glm::vec3 Forward();
	/** Right vector of the camera */
	glm::vec3 Right();

	glm::vec3 Position() const { return m_Position; }
	void SetPosition(glm::vec3 val);

	glm::quat Orientation() const { return m_Orientation; }
	void SetOrientation(glm::quat val);

	/*float Pitch() const { return m_Pitch; }
	void Pitch(float val);
	float Yaw() const { return m_Yaw; }
	void Yaw(float val);*/

	glm::mat4 ProjectionMatrix() const { return m_ProjectionMatrix; }
	glm::mat4 ViewMatrix() const { return m_ViewMatrix; }

	float AspectRatio() const { return m_AspectRatio; }
	void SetAspectRatio(float val);

	float FOV() const { return m_FOV; }
	void SetFOV(float val);

	float NearClip() const { return m_NearClip; }
	void SetNearClip(float val);

	float FarClip() const { return m_FarClip; }
	void SetFarClip(float val);

	float m_AspectRatio;
	float m_FOV;
	float m_NearClip;
	float m_FarClip;

private:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

	glm::vec3 m_Position;
	glm::quat m_Orientation;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
};

}

#endif // Camera_h__