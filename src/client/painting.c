#include <components.h>
#include <painting.h>
#include <raylib.h>

void CreatePainting(ecs_world_t* ctx, char* texture_path, float width)
{
    ecs_entity_t painting = ecs_new(ctx);

    Texture2D tex = LoadTexture(texture_path);

    ecs_set(ctx, painting, Position, { 0 });
    ecs_set_id(ctx, painting, ecs_id(Texture2D), sizeof(Texture2D), &tex);
    ecs_set(ctx, painting, TextureParams, {
                                              .size = { width, width * tex.height / tex.width },
                                              .source = { 0.f, 0.f, tex.width, tex.height },
                                          });
}
