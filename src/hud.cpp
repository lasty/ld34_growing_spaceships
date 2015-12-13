//
// Created by Tristan on 13/12/2015.
//

#include "hud.h"


#include "assets.h"

#include "globals.h"


HUD::HUD()
: font1(ASSETS->GetFont("hud"))
, text1(font1, "Hello Font World")
{

}


void HUD::Update(float dt)
{

}


void HUD::Render()
{

	text1.RenderSimple(10, 10);

}
