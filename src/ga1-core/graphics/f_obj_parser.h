#pragma once

/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/

/*
** Read an OBJ file, get the model data.
*/
void obj_to_model(const char* filename, struct ga_model* model, class Mesh* mesh);