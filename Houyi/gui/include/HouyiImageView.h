#ifndef HOUYIIMAGEVIEW_H_
#define HOUYIIMAGEVIEW_H_

#include "HouyiView.h"

namespace Houyi
{
	class ImageView : public View
	{
	public:
		enum FitType
		{
			EFill,
			EFitCenter,
			ECropCenter
		};
		ImageView();
		virtual ~ImageView();

		virtual void attachToScene(Scene* scene);
		virtual void setTexture(Texture* texture);
		virtual Texture* getTexture();
		virtual void setImage(ImagePtr image);
		virtual ImagePtr getImage();
        virtual void setFitType(FitType type);
        
        virtual bool onLayout(const HRect& bound, World* world);

        virtual void invalidate(HRect diry);

	protected:
        virtual void setTexcoord();

	protected:
        ImagePtr mImage;
        Texture* mTexture;

        FitType mFitType;
	};
}

#endif
