#include "HouyiFPCamera.h"
#include "HouyiScene.h"
#include "HouyiTextView.h"
#include "HouyiRoot.h"
#include "HouyiWorld.h"
#include "HouyiImageFactory.h"

namespace Houyi
{
    void FPCamera::prepare(Scene* scene)
    {
        mType = Const::EFirstPerson;
//        Root::getInstance()->addCommand(10000);
        scene->getRootSceneNode()->update();

        float maxx = -FLT_MAX;
        float maxy = -FLT_MAX;
        float maxz = -FLT_MAX;
        float minx = FLT_MAX;
        float miny = FLT_MAX;
        float minz = FLT_MAX;
        for (int i = 0; i < scene->getRenderableCount(); i++)
        {
            const RenderEntity& entity = scene->getRenderable(i);

            SceneNode* node = entity.mNode;
            SurfacePtr surface = entity.mSurface;

            VertexBufferPtr vb = surface->getVertexBuffer();
            if (!vb)
            {
                continue;
            }
            float* pos = vb->getPosition();
            for (int k = 0; k < vb->getPositionLength() / 3; k++)
            {
                Vector3 v = Vector3(pos[k * 3], pos[k * 3 + 1], pos[k * 3 + 2]);
                Vector3 vw = node->getLocal2World() * v;
                float x = vw.x;
                float y = vw.y;
                float z = vw.z;
                if (x > maxx)
                {
                    maxx = x;
                }
                if (x < minx)
                {
                    minx = x;
                }

                if (y > maxy)
                {
                    maxy = y;
                }
                if (y < miny)
                {
                    miny = y;
                }

                if (z > maxz)
                {
                    maxz = z;
                }
                if (z < minz)
                {
                    minz = z;
                }
            }
        }

        float cx = (maxx + minx) / 2;
        float cy = (maxy + miny) / 2;
        float cz = (maxz + minz) / 2;
        LOGD("ModelFPCamera::init(), cx = %f, cy = %f, cz = %f\n", cx, cy, cz);
        setLookAt(cx, cy, minz);
        float z = cz - cz / 4;
        setPosition(cx, cy, cz);
        mRadius = Math::Abs(minz - cz);
        LOGD("ModelFPCamera::init() Estimated z = %f\n", z);
        setFOV(90);
    }

    void FPCamera::addVirtualPan()
    {
//    	float density = getDensity();
//    	LOGE("density is %f", density);
//    	TextView* textViewF = HouyiNew TextView("F");
//    	HRect rectT(VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_WIDTH * 2 * density, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_HEIGHT * density);
//    	textViewF->setBound(rectT, TextView::EHAlignLeft, TextView::EVAlignBottom);
//    	textViewF->setOnTouchListener(this);
//    	textViewF->setId(FORWARD);
//    	Root::getInstance()->getWorld()->addView(textViewF);
//
//    	TextView* textViewL = HouyiNew TextView("L");
//    	HRect rectL(0, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_HEIGHT * density);
//    	textViewL->setBound(rectL, TextView::EHAlignLeft, TextView::EVAlignBottom);
//    	textViewL->setOnTouchListener(this);
//    	textViewL->setId(LEFT);
//    	Root::getInstance()->getWorld()->addView(textViewL);
//
//    	TextView* textViewR = HouyiNew TextView("R");
//    	HRect rectR(VIRTUAL_PAN_WIDTH * density * 2, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_HEIGHT * density);
//    	textViewR->setBound(rectR, TextView::EHAlignLeft, TextView::EVAlignBottom);
//    	textViewR->setOnTouchListener(this);
//    	textViewR->setId(RIGHT);
//    	Root::getInstance()->getWorld()->addView(textViewR);
//
//    	TextView* textViewB = HouyiNew TextView("B");
//    	HRect rectB(VIRTUAL_PAN_WIDTH * density, 0, VIRTUAL_PAN_WIDTH * density, VIRTUAL_PAN_HEIGHT * density);
//    	textViewB->setBound(rectB, TextView::EHAlignLeft, TextView::EVAlignBottom);
//    	textViewB->setOnTouchListener(this);
//    	textViewB->setId(BACK);
//    	Root::getInstance()->getWorld()->addView(textViewB);

    }

    bool FPCamera::onTouch(MotionEvent event, View* v) {
    	if(event.getType() == MotionEvent::TYPE_DOWN) {
        	mDirection |= v->getId();
    	} else if(event.getType() == MotionEvent::TYPE_UP){
    		mDirection = 0;
    	}

    	return true;
    }

    void FPCamera::update()
    {
		if (mDirection & FORWARD) {
			//forward
			moveForward(mAspectRatio, false);
			LOGE("moveForward");
		} else if (mDirection & LEFT) {
			//left
			moveLeft(mAspectRatio);
			LOGE("moveLeft");
		} else if (mDirection & RIGHT) {
			//right
			moveRight(mAspectRatio);
			LOGE("moveRight");
		} else if (mDirection & BACK) {
			//back
			moveBack(mAspectRatio, false);
			LOGE("moveBack");
		}
    }

    void FPCamera::rotate(const float x, const float y, const float z, const float radian)
    {
        mPosition -= mLookAt;
        mPosition.rotate(Vector3(x, y, z), radian);
        mPosition += mLookAt;
        mUp.rotate(Vector3(x, y, z), radian);
    }

    void FPCamera::rotate(const float angleH, const float angleV) {
    	Vector3 forward = getForward(false);
    	mLookAt = mPosition + forward.rotate(getRight(false), angleV);
    	lookAt();
    	forward = getForward(false);
    	mLookAt = mPosition +  forward.rotate(getUp(), angleH);
    	lookAt();
    }

    void FPCamera::moveForward(const float distance, bool currentDirection) {
    	if(currentDirection) {
        	Vector3 forward = getForward(true);
        	forward *= distance;
        	mPosition += forward;
        	mLookAt += forward;
    	} else {
        	Vector3 forward = getForward(false);
        	forward.y = mPosition.y;
        	forward.normalise();
        	forward *= distance;
        	mLookAt += forward;
        	mPosition += forward;
    	}
    }

    void FPCamera::moveBack(const float distance, bool currentDirection) {
    	if(currentDirection) {
        	Vector3 forward = getForward(true);
        	forward *= distance;
        	mPosition -= forward;
        	mLookAt -= forward;
    	} else {
        	Vector3 forward = getForward(false);
        	forward.y =  mPosition.y;
        	forward.normalise();
        	forward *= -distance;
        	mLookAt += forward;
        	mPosition += forward;
    	}
    }

    void FPCamera::moveLeft(const float distance) {
    	Vector3 right = getRight(true);
    	right *= distance;
    	mPosition -= right;
    	mLookAt -= right;
    }

    void FPCamera::moveRight(const float distance) {
    	Vector3 right = getRight(true);
    	right *= distance;
    	mPosition += right;
    	mLookAt += right;
    }
}
