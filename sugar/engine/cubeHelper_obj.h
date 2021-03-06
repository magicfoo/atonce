

char* name = "viewcube";

float locations[] = {
	// x, y, z
	-0.5000f, -0.5000f, 0.5000f,
	0.5000f, -0.5000f, 0.5000f,
	-0.5000f, -0.5000f, -0.5000f,
	0.5000f, -0.5000f, -0.5000f,
	-0.5000f, 0.5000f, 0.5000f,
	0.5000f, 0.5000f, 0.5000f,
	-0.5000f, 0.5000f, -0.5000f,
	0.5000f, 0.5000f, -0.5000f,
};

float uvs[] = {
	// x, y
	0.0100f, 0.0100f,
	0.3314f, 0.0100f,
	0.3314f, 0.3314f,
	0.0100f, 0.3314f,
	0.3393f, 0.0100f,
	0.6607f, 0.0100f,
	0.6607f, 0.3314f,
	0.3393f, 0.3314f,
	0.3393f, 0.6607f,
	0.3393f, 0.3393f,
	0.6607f, 0.3393f,
	0.6607f, 0.6607f,
	0.6686f, 0.0100f,
	0.9900f, 0.0100f,
	0.9900f, 0.3314f,
	0.6686f, 0.3314f,
	0.0100f, 0.3393f,
	0.3314f, 0.3393f,
	0.3314f, 0.6607f,
	0.0100f, 0.6607f,
	0.0100f, 0.6686f,
	0.3314f, 0.6686f,
	0.3314f, 0.9900f,
	0.0100f, 0.9900f,
};

float normals[] = {
	// x, y, z
	0.0f
};

int faces[] = {
	// v0-li,v0-ti,v0-ni, v1-li,v1-ti,v1-ni, v2-li,v2-ti,v2-ni
	3,10,-1, 1,11,-1, 0,8,-1, 
	0,8,-1, 2,9,-1, 3,10,-1, 
	7,22,-1, 6,23,-1, 4,20,-1, 
	4,20,-1, 5,21,-1, 7,22,-1, 
	5,6,-1, 4,7,-1, 0,4,-1, 
	0,4,-1, 1,5,-1, 5,6,-1, 
	7,14,-1, 5,15,-1, 1,12,-1, 
	1,12,-1, 3,13,-1, 7,14,-1, 
	6,18,-1, 7,19,-1, 3,16,-1, 
	3,16,-1, 2,17,-1, 6,18,-1, 
	4,2,-1, 6,3,-1, 2,0,-1, 
	2,0,-1, 0,1,-1, 4,2,-1, 
};


