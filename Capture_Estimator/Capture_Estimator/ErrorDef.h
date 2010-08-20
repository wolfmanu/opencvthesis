//Error definitions

#define CAM_NOT_CONNECTED		-10
#define FTP_CONNECTION_ERR		-11
#define FTP_USER_ERR			-12
#define FTP_PASSWOR_ERR			-13

#define MARKER_NOT_FOUND		-20
#define REPEATED_POSE			-21

#define CALIB_FILE_NOT_FOUND	-30
#define CALIB_FILE_READ_ERR		-31

////////////////////////////////////
//Other definitions

#define DEF_CAMERA_IP				"192.168.0.199"
#define DEF_IMG_FILE_BASE_NAME_SAVE "Saved_"
#define DEF_IMG_FILE_BASE_NAME_LOAD "image"
#define DEF_EXTENSION				".jpg"
#define DEF_SAVE_FOLDER				"."
#define DEF_LOAD_FOLDER				"."
#define DEF_IMG_FILE_START_NUM_LOAD 0
#define DEF_NUM_FRAME_TO_ELAB		INT_MAX
#define DEF_IMG_FILE_START_NUM_SAVE 0
#define DEF_USER					""
#define DEF_PASSWORD				""
#define DEF_SERVER					"localhost"
#define DEF_PORT					"21"
#define DEF_CALIB_FILE				"cameraTOOLBOX.cal"
#define DEF_CAM_H					480
#define DEF_CAM_W					704

#define NO_VIDEO					"-novideo"
#define USE_CAMERA					"-camera"
#define USE_LOAD					"-load"
#define USE_FTP						"-ftp"
#define USE_SAVE					"-save"
#define CAMERA_IP					"-ipcam"
#define IMG_FILE_BASE_NAME_SAVE		"-fbns"
#define IMG_FILE_BASE_NAME_LOAD		"-fbnl"
#define EXTENSION					"-ext"
#define SAVE_FOLDER					"-savefold"
#define LOAD_FOLDER					"-loadfold"
#define IMG_FILE_START_NUM_LOAD		"-nstartl"
#define IMG_FILE_START_NUM_SAVE		"-nstarts"
#define USER						"-u"
#define PASSWORD					"-pw"
#define SERVER						"-s"
#define PORT						"-p"
#define CALIB_FILE					"-calfile"
#define HELP						"-help"
#define HOW_MANY_LOAD				"-n"
#define WIDTH						"-w"
#define HEIGHT						"-h"