#pragma once
#include <vector>

class FluidParticle {
public:
     struct Particle {
		float x, y, z;      // Position (z=0 for 2D)
		float vx, vy, vz;   // Velocity (vz=0 for 2D)
		int material_id;    // Material type (e.g., water, oil), allows look up of liquid properties from database
		int phase_id;       // For multiphase fluids
     };

     FluidParticle(int nx, int ny, int nz = 1);
     Particle& at(int x, int y, int z = 0);
     float getXPos() const;
     float getYPos() const;
     float getZPos() const;
     float getVX() const;
     float getVY() const;
     float getVZ() const;
	int getMaterialID() const;
     int getPhaseID() const;
     // Add methods for setting/getting position, velocity, material ID, etc.
     void setPosition(int x, int y, int z, float posX, float posY, float posZ = 0.0f);
     void setVelocity(int x, int y, int z, float velX, float velY, float velZ = 0.0f);
     void setMaterialID(int x, int y, int z, int material_id);
	void setPhaseID(int x, int y, int z, int phase_id);

private:
     int width, height, depth;
     std::vector<Particle> Particles;
};