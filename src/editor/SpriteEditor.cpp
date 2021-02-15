#include <map>
#include "helpers/string_helper"
#include <vector>
#include "engine/datatypes/game_sprite.h"
#include "engine/rt/eng_types.h"
#include "helpers/OrderedMap.h"


using namespace std;
typedef std::pair<int,int> SPLICE;

struct coord_2d  
{
	int x;
	int y;

};

static enum CharEditorMode {
	CEDMODE_MOVE,
	CEDMODE_HITBOX,
	CEDMODE_OFFS,
	CEDMODE_TESSELATE
};
static enum HitBoxEdRegion {
	HEDREG_TOPLEFT,
	HEDREG_TOP,
	HEDREG_TOPRIGHT,
	HEDREG_LEFT,
	HEDREG_RIGHT,
	HEDREG_BOTTOMLEFT,
	HEDREG_BOTTOM,
	HEDREG_BOTTOMRIGHT
};

static enum HitBoxEdMode {
	HEDMODE_ADD,
	HEDMODE_MOVE,
	HEDMODE_RESIZE
};
static enum SplEdMode {
	SEDMODE_ADD,
	SEDMODE_RESIZE
};
struct HBAREARETVAL {
	int hbid;
	HitBoxEdRegion reg;
};
using ACTIONMAP = map<string, int>;
using int32 = unsigned int;
struct image_resource;

class FAnimated {
protected:
	int _CurFrame;
	int _LastUpdate;
	int _NumFrames;
public:

	FAnimated();
	void NextFrame();
	virtual void Tick(int t);
	int NumFrames();
	int LastUpdate();
	void ResetAnim();
	int CurFrame(){
		return this->_CurFrame;
	}
};

class FAnimation : public FAnimated {
	sprite_frame* _Frames;
public:
	

	FAnimation() {
		this->_Frames = NULL;
	}


	FAnimation(int nf) {
		if (nf == 0) {
			this->_Frames = NULL;
		} else {
			this->_Frames = new sprite_frame[nf];
		}
		this->_NumFrames = nf;
	}

	sprite_frame& operator[](int i) {
		return this->_Frames[i];
	}

 	/*operator int() {
 		return (int)this;
 	}*/
	
	operator void*() {
		return (void*)this;
	}

	//void operator delete[](void *p);

// 	void operator delete(void* _p) {
// 		Animation* p = (Animation*)_p;
// 		if (p->_Frames != NULL) {
// 			delete[] p->_Frames;
// 		}
// 	}

 	~FAnimation() {
 		if (this->_Frames != NULL) {
 			delete[] this->_Frames;
			this->_Frames = NULL;
 		}
 	}
	
	FAnimation& operator=(const FAnimation& a) {
		if (this != &a) {
			this->_CurFrame = a._CurFrame;
			this->_LastUpdate = a._LastUpdate;
			this->_NumFrames = a._NumFrames;
			if (this->_Frames != NULL) {
				delete[] this->_Frames;
				this->_Frames = NULL;
			}
			this->_Frames = new sprite_frame[a._NumFrames];
			for (int i = 0; i < a._NumFrames; i++) {
				this->_Frames[i] = a._Frames[i];
			}
		}
		return *this;
	}
	FAnimation(const FAnimation& a) {
		if (this != &a) {
			this->_CurFrame = a._CurFrame;
			this->_LastUpdate = a._LastUpdate;
			this->_NumFrames = a._NumFrames;
			this->_Frames = new sprite_frame[a._NumFrames];
			for (int i = 0; i < a._NumFrames; i++) {
				this->_Frames[i] = a._Frames[i];
			}
		}
	}


	sprite_frame* Frames() {
		return this->_Frames;
	}

	void RemoveFrame(int index) {
		int dsize = this->_NumFrames-1;
		sprite_frame* tmp = new sprite_frame[dsize];
		int j = 0;
		for (int i = 0; i < this->_NumFrames; i++) {
			if (i != index) {
				tmp[j] = this->_Frames[i];
				j++;
			}
		}
		delete[] this->_Frames;
		this->_Frames = tmp;
		this->_NumFrames--;
		this->ResetAnim();
	}

	void AddFrame() {
		int dsize = this->_NumFrames+1;
		sprite_frame* tmp = new sprite_frame[dsize];
		for (int i = 0; i < this->_NumFrames; i++) {
			tmp[i] = this->_Frames[i];
		}
		delete[] this->_Frames;
		this->_Frames = tmp;
		this->_NumFrames++;
	}

	void AddFrame(int before) {
		int dsize = this->_NumFrames+1;
		sprite_frame* tmp = new sprite_frame[dsize];
		int j = 0;
		for (int i = 0; i <= this->_NumFrames; i++) {
			if (i != before) {
				tmp[i] = this->_Frames[j];
				j++;
			}
		}
		delete[] this->_Frames;
		this->_Frames = tmp;
		this->_NumFrames++;
	}
};

class CharEditor {
	// constants
	static const int DEFHB_W;
	static const int DEFHB_H;
	static const int32 INVALID_HITBOX;
	static const int32 INVALID_SPLICE;
	static const int32 INVALID_UNDO;
	static const int MAXZOOMF;
	static const int MAXZOOMA;
	static const int HBAREA_THRS;
    static OrderedMap<string, game_sprite*> WorkingSpr;
	// action list
	static vector<string> OldList;
	static ACTIONMAP MaxAction;
	static game_sprite* CurSprite;
	static FAnimation* CurAnim;
	static sprite_frame* CurFrame;
	static image_resource* CurImg;
	static int CurAct;
	static int FrameIndex;
	static int ZoomFactorA;
	static int ZoomFactorF;
	static coord_2d CurCam;
	static coord_2d AnimCam;
	static CharEditorMode Mode;
	static HitBoxEdMode HbMode;
	static SplEdMode SpMode;
	static int HoverHB;
	static bool HasFrame;
	static int HoverSP;
	static HBAREARETVAL HoverHBResArea;
	static int CharEdTimer;
	
	//temporaries
	static eng_rect ActiveHitbox;
	static SPLICE ActiveSplice;

	// gdi shit
	/*static HBITMAP CurDIB;
	static HDC CompatDC;
	static HDC TessDC;
	static HBITMAP TessBMP;
	static HPEN CrossPen;
	static const COLORREF TessColors[4];
	static HBRUSH TessBrush[4];
	static std::vector<CharEdCmd*> CmdList;
	static int32 CmdPointer;
	static POINT clicked_point;
	static HBRUSH green_brush;
	static HBRUSH blue_brush;
	static HBRUSH yellow_brush;*/
};