#include "ToolMain.h"
#include "resource.h"
#include <vector>
#include <sstream>

//
//ToolMain Class
ToolMain::ToolMain()
{

	m_currentChunk = 0;		//default value
	m_selectedObject = 0;	//initial selection ID
	m_sceneGraph.clear();	//clear the vector for the scenegraph
	m_databaseConnection = NULL;

	//zero input commands
	m_toolInputCommands.forward		= false;
	m_toolInputCommands.back		= false;
	m_toolInputCommands.left		= false;
	m_toolInputCommands.right		= false;
	m_toolInputCommands.ascend = false;
	m_toolInputCommands.descend = false;

	m_toolInputCommands.rotRight = false;
	m_toolInputCommands.rotLeft = false;
	m_toolInputCommands.rotUp = false;
	m_toolInputCommands.rotDown = false;

	m_toolInputCommands.mouse_X = 0;
	m_toolInputCommands.mouse_Y = 0;
	m_toolInputCommands.mouse_LB_Down = false;
	m_toolInputCommands.mouse_LB_Up = false;
	m_toolInputCommands.mouse_LB_DoubleClickTime = 0.0f;

	m_toolInputCommands.mode_terrainEditor = false;
	m_toolInputCommands.completeCineCam = false;
	m_toolInputCommands.createCineCam = false;
	m_toolInputCommands.switchCam = false;

	m_toolInputCommands.objDown = false;
	m_toolInputCommands.objUp = false;
	m_toolInputCommands.objRight = false;
	m_toolInputCommands.objLeft = false;

	m_toolInputCommands.mode_rotate = false;
	m_toolInputCommands.mode_translate = true;
	m_toolInputCommands.mode_scale = false;
	m_toolInputCommands.switchObjMode = false;
	
}


ToolMain::~ToolMain()
{
	sqlite3_close(m_databaseConnection);		//close the database connection
}


int ToolMain::getCurrentSelectionID()
{

	return m_selectedObject;
}

void ToolMain::onActionInitialise(HWND handle, int width, int height)
{
	//window size, handle etc for directX
	m_width		= width;
	m_height	= height;
	
	m_d3dRenderer.Initialize(handle, m_width, m_height);

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db",&m_databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc) 
	{
		TRACE("Can't open database");
		//if the database cant open. Perhaps a more catastrophic error would be better here
	}
	else 
	{
		TRACE("Opened database successfully");
	}

	onActionLoad();
}

void ToolMain::onActionLoad()
{
	//load current chunk and objects into lists
	if (!m_sceneGraph.empty())		//is the vector empty
	{
		m_sceneGraph.clear();		//if not, empty it
	}

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	sqlite3_stmt *pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects";				//sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0 );
	
	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{	
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.model_path		= reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2));
		newSceneObject.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.collision_mesh = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.audio_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);
	

		//send completed object to scenegraph
		m_sceneGraph.push_back(newSceneObject);
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";				//sql command which will return all records from  chunks table. There is only one tho.
														//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResultsChunk, 0);


	sqlite3_step(pResultsChunk);
	m_chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	m_chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	m_chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	m_chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	m_chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	m_chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	m_chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	m_chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	m_chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	m_chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	m_chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	m_chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	m_chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	m_chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	m_chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	m_chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	m_chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	m_chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	m_chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);


	//Process REsults into renderable
	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	//build the renderable chunk 
	m_d3dRenderer.BuildDisplayChunk(&m_chunk);

}

void ToolMain::onActionSave()
{
	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	

	//OBJECTS IN THE WORLD Delete them all
	//prepare SQL Text
	sqlCommand = "DELETE FROM Objects";	 //will delete the whole object table.   Slightly risky but hey.
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	sqlite3_step(pResults);

	//Populate with our new objects
	std::wstring sqlCommand2;
	int numObjects = m_sceneGraph.size();	//Loop thru the scengraph.

	for (int i = 0; i < numObjects; i++)
	{
		std::stringstream command;
		command << "INSERT INTO Objects " 
			<<"VALUES(" << m_sceneGraph.at(i).ID << ","
			<< m_sceneGraph.at(i).chunk_ID  << ","
			<< "'" << m_sceneGraph.at(i).model_path <<"'" << ","
			<< "'" << m_sceneGraph.at(i).tex_diffuse_path << "'" << ","
			<< m_sceneGraph.at(i).posX << ","
			<< m_sceneGraph.at(i).posY << ","
			<< m_sceneGraph.at(i).posZ << ","
			<< m_sceneGraph.at(i).rotX << ","
			<< m_sceneGraph.at(i).rotY << ","
			<< m_sceneGraph.at(i).rotZ << ","
			<< m_sceneGraph.at(i).scaX << ","
			<< m_sceneGraph.at(i).scaY << ","
			<< m_sceneGraph.at(i).scaZ << ","
			<< m_sceneGraph.at(i).render << ","
			<< m_sceneGraph.at(i).collision << ","
			<< "'" << m_sceneGraph.at(i).collision_mesh << "'" << ","
			<< m_sceneGraph.at(i).collectable << ","
			<< m_sceneGraph.at(i).destructable << ","
			<< m_sceneGraph.at(i).health_amount << ","
			<< m_sceneGraph.at(i).editor_render << ","
			<< m_sceneGraph.at(i).editor_texture_vis << ","
			<< m_sceneGraph.at(i).editor_normals_vis << ","
			<< m_sceneGraph.at(i).editor_collision_vis << ","
			<< m_sceneGraph.at(i).editor_pivot_vis << ","
			<< m_sceneGraph.at(i).pivotX << ","
			<< m_sceneGraph.at(i).pivotY << ","
			<< m_sceneGraph.at(i).pivotZ << ","
			<< m_sceneGraph.at(i).snapToGround << ","
			<< m_sceneGraph.at(i).AINode << ","
			<< "'" << m_sceneGraph.at(i).audio_path << "'" << ","
			<< m_sceneGraph.at(i).volume << ","
			<< m_sceneGraph.at(i).pitch << ","
			<< m_sceneGraph.at(i).pan << ","
			<< m_sceneGraph.at(i).one_shot << ","
			<< m_sceneGraph.at(i).play_on_init << ","
			<< m_sceneGraph.at(i).play_in_editor << ","
			<< m_sceneGraph.at(i).min_dist << ","
			<< m_sceneGraph.at(i).max_dist << ","
			<< m_sceneGraph.at(i).camera << ","
			<< m_sceneGraph.at(i).path_node << ","
			<< m_sceneGraph.at(i).path_node_start << ","
			<< m_sceneGraph.at(i).path_node_end << ","
			<< m_sceneGraph.at(i).parent_id << ","
			<< m_sceneGraph.at(i).editor_wireframe << ","
			<< "'" << m_sceneGraph.at(i).name << "'" << ","

			<< m_sceneGraph.at(i).light_type << ","
			<< m_sceneGraph.at(i).light_diffuse_r << ","
			<< m_sceneGraph.at(i).light_diffuse_g << ","
			<< m_sceneGraph.at(i).light_diffuse_b << ","
			<< m_sceneGraph.at(i).light_specular_r << ","
			<< m_sceneGraph.at(i).light_specular_g << ","
			<< m_sceneGraph.at(i).light_specular_b << ","
			<< m_sceneGraph.at(i).light_spot_cutoff << ","
			<< m_sceneGraph.at(i).light_constant << ","
			<< m_sceneGraph.at(i).light_linear << ","
			<< m_sceneGraph.at(i).light_quadratic

			<< ")";
		std::string sqlCommand2 = command.str();
		rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand2.c_str(), -1, &pResults, 0);
		sqlite3_step(pResults);	
	}
	MessageBox(NULL, L"Objects Saved", L"Notification", MB_OK);
}

void ToolMain::onActionSaveTerrain()
{
	m_d3dRenderer.SaveDisplayChunk(&m_chunk);
}

void ToolMain::setEditor()
{
	m_toolInputCommands.mode_terrainEditor = !m_toolInputCommands.mode_terrainEditor;
}

void ToolMain::switchObjMode()
{
	m_toolInputCommands.switchObjMode = true; 
}

void ToolMain::switchCamMode(CamType newType)
{
	m_d3dRenderer.m_CameraManager->swichCamType(newType);

}

void ToolMain::switchCamNum()
{
	m_d3dRenderer.m_CameraManager->swichcam();
}

void ToolMain::Tick(MSG *msg)
{
	if (m_toolInputCommands.switchCam) {
		m_d3dRenderer.m_CameraManager->swichcam();
	}

	if (m_toolInputCommands.switchObjMode) {//had mode switching for object manipulation on a key press so this may be obsolete now 
		m_toolInputCommands.switchObjMode = false;
		if (m_toolInputCommands.mode_translate) {
			m_toolInputCommands.mode_translate = false;
			m_toolInputCommands.mode_rotate = true;
			return;
		}		
		if (m_toolInputCommands.mode_rotate) {
			m_toolInputCommands.mode_rotate = false;
			m_toolInputCommands.mode_scale = true;
			return;
		}		
		if (m_toolInputCommands.mode_scale) {
			m_toolInputCommands.mode_scale = false;
			m_toolInputCommands.mode_translate = true;
			return;
		}
		//m_toolInputCommands.switchObjMode = false;
	}

	//OBJECT MANIPULATION START  
	if (m_d3dRenderer.m_lastID != -1) {
		if (m_toolInputCommands.objUp) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posY += .1f;
			
			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotY += 1.f;
			
			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).scaY += .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);

		}
		
		if (m_toolInputCommands.objDown) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posY -= .1f;

			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotY -= 1.f;

			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_selectedObject).scaY -= .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);
		}		
		
		if (m_toolInputCommands.objForward) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posZ += .1f;

			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotZ += 1.f;

			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).scaZ += .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);
		}	

		if (m_toolInputCommands.objBack) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posZ -= .1f;

			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotZ -= 1.f;

			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).scaZ -= .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);
		}		
		
		if (m_toolInputCommands.objRight) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posX += .1f;

			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotX += 1.f;

			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).scaX += .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);
		}	

		if (m_toolInputCommands.objLeft) {
			if (m_toolInputCommands.mode_translate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).posX -= .1f;

			if (m_toolInputCommands.mode_rotate)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).rotX -= 1.f;

			if (m_toolInputCommands.mode_scale)
				m_sceneGraph.at(m_d3dRenderer.m_lastID).scaX -= .1f;

			m_d3dRenderer.updateObj(m_sceneGraph.at(m_d3dRenderer.m_lastID), m_d3dRenderer.m_lastID);
		}
	}
	//OBJECT MANIPULATION END 

	//COPY PASTE START 
	if (m_toolInputCommands.copy) {
		m_toolInputCommands.copy = false;
		if(m_selectedObject != -1)
			m_d3dRenderer.copyObj(m_selectedObject);
	}

	if (m_toolInputCommands.paste) {
		if (m_toolInputCommands.pasteDelay <= 0.0f) {
			m_d3dRenderer.pasteObj(m_sceneGraph);
			m_toolInputCommands.pasteDelay = 10.0f;	// a delay was added for pasting so hundreds of objects are not created on a single key press 
		}
	}

	if (m_toolInputCommands.pasteDelay > 0.0f)
		m_toolInputCommands.pasteDelay -= 0.5f;
	//COPY PASTE END 

	//WIREFRAME MODE START
	if (m_toolInputCommands.mode_wireFrame) {
		if (m_toolInputCommands.wireFrameDelay <= 0.0f) {
			m_d3dRenderer.m_wireFrame = !m_d3dRenderer.m_wireFrame;	//delay added for wireframe so the mode isn't toggled multiple times in a single key press 
			m_toolInputCommands.wireFrameDelay = 10.0f;
		}
	}

	if (m_toolInputCommands.wireFrameDelay > 0.0f)
		m_toolInputCommands.wireFrameDelay -= 0.5f;
	//WIREFRAME MODE END 

	//MOUSE CONTROLS START 
	if (m_toolInputCommands.mouse_LB_DoubleClickTime > 0.0f)
		m_toolInputCommands.mouse_LB_DoubleClickTime -= 0.1f;

	if (m_toolInputCommands.mouse_LB_Down)
	{
		if (!m_toolInputCommands.mode_terrainEditor) {
			if (m_secondObject == m_selectedObject) {
				m_thirdObject = m_d3dRenderer.MousePicking();
				if (m_thirdObject == m_selectedObject) {
					m_d3dRenderer.TripleLClick(m_selectedObject);
					m_thirdObject = -1;
				}
			}

			if (m_toolInputCommands.mouse_LB_DoubleClickTime > 0.0f)
			{
				//double click
				m_secondObject = m_d3dRenderer.MousePicking();
				//m_d3dRenderer.chunk();

				//check if second click hits the same object as the first 
				if (m_secondObject == m_selectedObject)
					m_d3dRenderer.DoubleLClick(m_selectedObject);

				if (m_secondObject != m_selectedObject) {
					m_selectedObject = m_secondObject;
					m_toolInputCommands.mouse_LB_DoubleClickTime = 0.0f;
				}
				m_toolInputCommands.mouse_LB_Down = false;
			}

			if (m_toolInputCommands.mouse_LB_DoubleClickTime <= 0.0f)
			{
				//single click 
				m_toolInputCommands.mouse_LB_DoubleClickTime = 10.0f;

				m_selectedObject = m_d3dRenderer.MousePicking();
				m_toolInputCommands.mouse_LB_Down = false;
				return;
			}
		}

		//if currently in terrain editor mode do terrain editor stuff
		if (m_toolInputCommands.mode_terrainEditor) {	 
			m_d3dRenderer.chunk();
			m_toolInputCommands.mouse_LB_Down = false;


			if (m_toolInputCommands.mouse_LB_Up == true) {
				m_toolInputCommands.mouse_LB_Up = false;
				m_toolInputCommands.mouse_LB_Down = false;
			}
		}
	}
	//MOUSE CONTROLS END


	//Renderer Update Call
	m_d3dRenderer.Tick(&m_toolInputCommands);
}

void ToolMain::UpdateInput(MSG * msg)
{

	switch (msg->message)
	{
		//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		m_keyArray[msg->wParam] = true;
		break;

	case WM_KEYUP:
		m_keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:
		//update the mouse X and Y which will be sent thru to the Renderer.
		m_toolInputCommands.mouse_X = GET_X_LPARAM(msg->lParam);
		m_toolInputCommands.mouse_Y = GET_Y_LPARAM(msg->lParam);
		break;

	case WM_LBUTTONDOWN:
		//mouse left pressed.	
		m_toolInputCommands.mouse_LB_Down = true;
		m_toolInputCommands.mouse_LB_Up = false;
		break;

	case WM_LBUTTONUP:
		m_toolInputCommands.mouse_LB_Up = true;
		break;
	}
	//here we update all the actual app functionality that we want.  This information will either be used int toolmain, or sent down to the renderer (Camera movement etc
	//WASD movement
	if (m_keyArray['W'])
	{
		m_toolInputCommands.forward = true;
	}
	else m_toolInputCommands.forward = false;
	
	if (m_keyArray['S'])
	{
		m_toolInputCommands.back = true;
	}
	else m_toolInputCommands.back = false;
	if (m_keyArray['A'])
	{
		m_toolInputCommands.left = true;
	}
	else m_toolInputCommands.left = false;

	if (m_keyArray['D'])
	{
		m_toolInputCommands.right = true;
	}
	else m_toolInputCommands.right = false;
	//rotation
	if (m_keyArray['E'])
	{
		m_toolInputCommands.rotRight = true;
	}
	else m_toolInputCommands.rotRight = false;
	if (m_keyArray['Q'])
	{
		m_toolInputCommands.rotLeft = true;
	}
	else m_toolInputCommands.rotLeft = false;
	if (m_keyArray['R'])
	{
		m_toolInputCommands.rotUp = true;
	}
	else m_toolInputCommands.rotUp = false;	
	if (m_keyArray['F'])
	{
		m_toolInputCommands.rotDown = true;
	}
	else m_toolInputCommands.rotDown = false;	
	if (m_keyArray['P'])
	{
		m_toolInputCommands.mode_wireFrame = true;
	}
	else m_toolInputCommands.mode_wireFrame = false;	
	if (m_keyArray[' '])
	{
		m_toolInputCommands.ascend = true;
	}
	else m_toolInputCommands.ascend = false;	
	if (m_keyArray['Z'])
	{
		m_toolInputCommands.descend	 = true;
	}
	else m_toolInputCommands.descend = false;	
	if (m_keyArray['N'])
	{
		m_toolInputCommands.createCineCam = true;
	}
	else m_toolInputCommands.createCineCam = false;	
	if (m_keyArray['0'])
	{
		m_toolInputCommands.createStillCam = true;
	}
	else m_toolInputCommands.createStillCam = false;
	if (m_keyArray['C'])
	{
		m_toolInputCommands.copy = true;
	}
	else m_toolInputCommands.copy = false;	
	if (m_keyArray['V'])
	{
		m_toolInputCommands.paste = true;
	}
	else m_toolInputCommands.paste = false;	
	if (m_keyArray['M'])
	{
		m_toolInputCommands.switchCam = true;
	}
	else m_toolInputCommands.switchCam = false;	
	if (m_keyArray['B'])
	{
		m_toolInputCommands.completeCineCam = true;
	}
	else m_toolInputCommands.completeCineCam = false;	
	if (m_keyArray['I'])
	{
		m_toolInputCommands.objUp = true;
	}
	else m_toolInputCommands.objUp = false;	
	if (m_keyArray['K'])
	{
		m_toolInputCommands.objDown = true;
	}
	else m_toolInputCommands.objDown = false;	
	if (m_keyArray['J'])
	{
		m_toolInputCommands.objLeft = true;
	}
	else m_toolInputCommands.objLeft = false;
	if (m_keyArray['L'])
	{
		m_toolInputCommands.objRight = true;
	}
	else m_toolInputCommands.objRight = false;
	if (m_keyArray['U'])
	{
		m_toolInputCommands.objForward = true;
	}
	else m_toolInputCommands.objForward = false;
	if (m_keyArray['O'])
	{
		m_toolInputCommands.objBack = true;
	}
	else m_toolInputCommands.objBack = false;	
//	if (m_keyArray['T'])
	//{
		//m_toolInputCommands.switchObjMode = true;
	//}
	//else m_toolInputCommands.switchObjMode = false;
}
