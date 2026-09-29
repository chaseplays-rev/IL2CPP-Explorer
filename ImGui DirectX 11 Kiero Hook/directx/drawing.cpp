#include "drawing.h"

namespace Drawing
{
   /* void DrawCachedObjects(ImDrawList* pDrawList)
    {
        if (!Globals::highlightObj ||
            !Explorer::pSelectedClass)
        {
            return;
        }

        if (!Cache::objectsHandle)
            return;

        Camera* pCam =
            Camera::Get();

        if (!pCam)
            return;

        Array<Object*>* pObjects =
            reinterpret_cast<Array<Object*>*>(
                mono_gchandle_get_target_v2(
                    Cache::objectsHandle
                )
                );

        if (!pObjects)
            return;

        int count =
            pObjects->GetLength();

        if (count <= 0)
            return;

        for (int i = 0; i < count; i++)
        {
            Component* object =
                reinterpret_cast<Component*>(
                    pObjects->GetValue(i)
                    );

            if (!object)
                continue;

            if (!object->IsValid())
                continue;

            Transform* pTrans =
                object->GetTransform();

            if (!pTrans)
                continue;

            Vector3 pos =
                pTrans->GetPosition();

            Vector2 screenPos;

            if (!pCam->WorldToScreen(
                screenPos,
                pos
            ))
            {
                continue;
            }

            std::string objTextFull =
                std::to_string(i) +
                " - " +
                Explorer::pSelectedClass->GetName();

            pDrawList->AddText(
                {
                    screenPos.x,
                    screenPos.y
                },
                ImColor(
                    255,
                    255,
                    255
                ),
                objTextFull.c_str()
            );
        }
    }*/

    void Render(ImDrawList* pDrawList)
    {
        /*DrawCachedObjects(
            pDrawList
        );*/
    }
}