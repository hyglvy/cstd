
//=====================================================================
// ��ѧ�⣺�˲���Ӧ�ò�����⣬��Ϥ D3D ����任����
//=====================================================================
typedef struct {
  float m[4][4];
} matrix_t;
typedef struct {
  float x, y, z, w;
} vector_t;
typedef vector_t point_t;

int CMID(int x, int min, int max)
{
  return (x < min) ? min : ((x > max) ? max : x);
}

// �����ֵ��t Ϊ [0, 1] ֮�����ֵ
float interp(float x1, float x2, float t)
{
  return x1 + (x2 - x1) * t;
}

// | v |
float vector_length(const vector_t* v)
{
  float sq = v->x * v->x + v->y * v->y + v->z * v->z;
  return (float)sqrt(sq);
}

// z = x + y
void vector_add(vector_t* z, const vector_t* x, const vector_t* y)
{
  z->x = x->x + y->x;
  z->y = x->y + y->y;
  z->z = x->z + y->z;
  z->w = 1.0;
}

// z = x - y
void vector_sub(vector_t* z, const vector_t* x, const vector_t* y)
{
  z->x = x->x - y->x;
  z->y = x->y - y->y;
  z->z = x->z - y->z;
  z->w = 1.0;
}

// ʸ�����
float vector_dotproduct(const vector_t* x, const vector_t* y)
{
  return x->x * y->x + x->y * y->y + x->z * y->z;
}

// ʸ�����
void vector_crossproduct(vector_t* z, const vector_t* x, const vector_t* y)
{
  float m1, m2, m3;
  m1 = x->y * y->z - x->z * y->y;
  m2 = x->z * y->x - x->x * y->z;
  m3 = x->x * y->y - x->y * y->x;
  z->x = m1;
  z->y = m2;
  z->z = m3;
  z->w = 1.0f;
}

// ʸ����ֵ��tȡֵ [0, 1]
void vector_interp(vector_t* z, const vector_t* x1, const vector_t* x2, float t)
{
  z->x = interp(x1->x, x2->x, t);
  z->y = interp(x1->y, x2->y, t);
  z->z = interp(x1->z, x2->z, t);
  z->w = 1.0f;
}

// ʸ����һ��
void vector_normalize(vector_t* v)
{
  float length = vector_length(v);

  if (length != 0.0f) {
    float inv = 1.0f / length;
    v->x *= inv;
    v->y *= inv;
    v->z *= inv;
  }
}

// c = a + b
void matrix_add(matrix_t* c, const matrix_t* a, const matrix_t* b)
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      c->m[i][j] = a->m[i][j] + b->m[i][j];
    }
  }
}

// c = a - b
void matrix_sub(matrix_t* c, const matrix_t* a, const matrix_t* b)
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      c->m[i][j] = a->m[i][j] - b->m[i][j];
    }
  }
}

// c = a * b
void matrix_mul(matrix_t* c, const matrix_t* a, const matrix_t* b)
{
  matrix_t z;
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      z.m[j][i] = (a->m[j][0] * b->m[0][i]) +
          (a->m[j][1] * b->m[1][i]) +
          (a->m[j][2] * b->m[2][i]) +
          (a->m[j][3] * b->m[3][i]);
    }
  }

  c[0] = z;
}

// c = a * f
void matrix_scale(matrix_t* c, const matrix_t* a, float f)
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      c->m[i][j] = a->m[i][j] * f;
    }
  }
}

// y = x * m
void matrix_apply(vector_t* y, const vector_t* x, const matrix_t* m)
{
  float x = x->x, y = x->y, z = x->z, w = x->w;
  y->x = x * m->m[0][0] + y * m->m[1][0] + z * m->m[2][0] + w * m->m[3][0];
  y->y = x * m->m[0][1] + y * m->m[1][1] + z * m->m[2][1] + w * m->m[3][1];
  y->z = x * m->m[0][2] + y * m->m[1][2] + z * m->m[2][2] + w * m->m[3][2];
  y->w = x * m->m[0][3] + y * m->m[1][3] + z * m->m[2][3] + w * m->m[3][3];
}

void matrix_set_identity(matrix_t* m)
{
  m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
  m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
  m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
  m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
  m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

void matrix_set_zero(matrix_t* m)
{
  m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
  m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
  m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
  m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}

// ƽ�Ʊ任
void matrix_set_translate(matrix_t* m, float x, float y, float z)
{
  matrix_set_identity(m);
  m->m[3][0] = x;
  m->m[3][1] = y;
  m->m[3][2] = z;
}

// ���ű任
void matrix_set_scale(matrix_t* m, float x, float y, float z)
{
  matrix_set_identity(m);
  m->m[0][0] = x;
  m->m[1][1] = y;
  m->m[2][2] = z;
}

// ��ת����
void matrix_set_rotate(matrix_t* m, float x, float y, float z, float theta)
{
  float qsin = (float)sin(theta * 0.5f);
  float qcos = (float)cos(theta * 0.5f);
  vector_t vec = { x, y, z, 1.0f };
  float w = qcos;
  vector_normalize(&vec);
  x = vec.x * qsin;
  y = vec.y * qsin;
  z = vec.z * qsin;
  m->m[0][0] = 1 - 2 * y * y - 2 * z * z;
  m->m[1][0] = 2 * x * y - 2 * w * z;
  m->m[2][0] = 2 * x * z + 2 * w * y;
  m->m[0][1] = 2 * x * y + 2 * w * z;
  m->m[1][1] = 1 - 2 * x * x - 2 * z * z;
  m->m[2][1] = 2 * y * z - 2 * w * x;
  m->m[0][2] = 2 * x * z - 2 * w * y;
  m->m[1][2] = 2 * y * z + 2 * w * x;
  m->m[2][2] = 1 - 2 * x * x - 2 * y * y;
  m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
  m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
  m->m[3][3] = 1.0f;
}

// ���������
void matrix_set_lookat(matrix_t* m, const vector_t* eye, const vector_t* at, const vector_t* up)
{
  vector_t xaxis, yaxis, zaxis;

  vector_sub(&zaxis, at, eye);
  vector_normalize(&zaxis);
  vector_crossproduct(&xaxis, up, &zaxis);
  vector_normalize(&xaxis);
  vector_crossproduct(&yaxis, &zaxis, &xaxis);

  m->m[0][0] = xaxis.x;
  m->m[1][0] = xaxis.y;
  m->m[2][0] = xaxis.z;
  m->m[3][0] = -vector_dotproduct(&xaxis, eye);

  m->m[0][1] = yaxis.x;
  m->m[1][1] = yaxis.y;
  m->m[2][1] = yaxis.z;
  m->m[3][1] = -vector_dotproduct(&yaxis, eye);

  m->m[0][2] = zaxis.x;
  m->m[1][2] = zaxis.y;
  m->m[2][2] = zaxis.z;
  m->m[3][2] = -vector_dotproduct(&zaxis, eye);

  m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
  m->m[3][3] = 1.0f;
}

// D3DXMatrixPerspectiveFovLH
void matrix_set_perspective(matrix_t* m, float fovy, float aspect, float zn, float zf)
{
  float fax = 1.0f / (float)tan(fovy * 0.5f);
  matrix_set_zero(m);
  m->m[0][0] = (float)(fax / aspect);
  m->m[1][1] = (float)(fax);
  m->m[2][2] = zf / (zf - zn);
  m->m[3][2] = - zn * zf / (zf - zn);
  m->m[2][3] = 1;
}


//=====================================================================
// ����任
//=====================================================================
typedef struct {
  matrix_t world;         // ��������任
  matrix_t view;          // ��Ӱ������任
  matrix_t projection;    // ͶӰ�任
  matrix_t transform;     // transform = world * view * projection
  float w, h;             // ��Ļ��С
} transform_t;


// ������£����� transform = world * view * projection
void transform_update(transform_t* ts)
{
  matrix_t m;
  matrix_mul(&m, &ts->world, &ts->view);
  matrix_mul(&ts->transform, &m, &ts->projection);
}

// ��ʼ����������Ļ����
void transform_init(transform_t* ts, int w, int h)
{
  float aspect = (float)w / ((float)h);
  matrix_set_identity(&ts->world);
  matrix_set_identity(&ts->view);
  matrix_set_perspective(&ts->projection, 3.1415926f * 0.5f, aspect, 1.0f, 500.0f);
  ts->w = (float)w;
  ts->h = (float)h;
  transform_update(ts);
}

// ��ʸ�� x ���� project
void transform_apply(const transform_t* ts, vector_t* y, const vector_t* x)
{
  matrix_apply(y, x, &ts->transform);
}

// ����������ͬ cvv �ı߽�������׶�ü�
int transform_check_cvv(const vector_t* v)
{
  float w = v->w;
  int check = 0;

  if (v->z < 0.0f) {
    check |= 1;
  }

  if (v->z >  w) {
    check |= 2;
  }

  if (v->x < -w) {
    check |= 4;
  }

  if (v->x >  w) {
    check |= 8;
  }

  if (v->y < -w) {
    check |= 16;
  }

  if (v->y >  w) {
    check |= 32;
  }

  return check;
}

// ��һ�����õ���Ļ����
void transform_homogenize(const transform_t* ts, vector_t* y, const vector_t* x)
{
  float rhw = 1.0f / x->w;
  y->x = (x->x * rhw + 1.0f) * ts->w * 0.5f;
  y->y = (1.0f - x->y * rhw) * ts->h * 0.5f;
  y->z = x->z * rhw;
  y->w = 1.0f;
}


//=====================================================================
// ���μ��㣺���㡢ɨ���ߡ���Ե�����Ρ���������
//=====================================================================
typedef struct {
  float r, g, b;
} color_t;
typedef struct {
  float u, v;
} texcoord_t;
typedef struct {
  point_t pos;
  texcoord_t tc;
  color_t color;
  float rhw;
} vertex_t;

typedef struct {
  vertex_t v, v1, v2;
} edge_t;
typedef struct {
  float top, bottom;
  edge_t left, right;
} trapezoid_t;
typedef struct {
  vertex_t v, step;
  int x, y, w;
} scanline_t;


void vertex_rhw_init(vertex_t* v)
{
  float rhw = 1.0f / v->pos.w;
  v->rhw = rhw;
  v->tc.u *= rhw;
  v->tc.v *= rhw;
  v->color.r *= rhw;
  v->color.g *= rhw;
  v->color.b *= rhw;
}

void vertex_interp(vertex_t* y, const vertex_t* x1, const vertex_t* x2, float t)
{
  vector_interp(&y->pos, &x1->pos, &x2->pos, t);
  y->tc.u = interp(x1->tc.u, x2->tc.u, t);
  y->tc.v = interp(x1->tc.v, x2->tc.v, t);
  y->color.r = interp(x1->color.r, x2->color.r, t);
  y->color.g = interp(x1->color.g, x2->color.g, t);
  y->color.b = interp(x1->color.b, x2->color.b, t);
  y->rhw = interp(x1->rhw, x2->rhw, t);
}

void vertex_division(vertex_t* y, const vertex_t* x1, const vertex_t* x2, float w)
{
  float inv = 1.0f / w;
  y->pos.x = (x2->pos.x - x1->pos.x) * inv;
  y->pos.y = (x2->pos.y - x1->pos.y) * inv;
  y->pos.z = (x2->pos.z - x1->pos.z) * inv;
  y->pos.w = (x2->pos.w - x1->pos.w) * inv;
  y->tc.u = (x2->tc.u - x1->tc.u) * inv;
  y->tc.v = (x2->tc.v - x1->tc.v) * inv;
  y->color.r = (x2->color.r - x1->color.r) * inv;
  y->color.g = (x2->color.g - x1->color.g) * inv;
  y->color.b = (x2->color.b - x1->color.b) * inv;
  y->rhw = (x2->rhw - x1->rhw) * inv;
}

void vertex_add(vertex_t* y, const vertex_t* x)
{
  y->pos.x += x->pos.x;
  y->pos.y += x->pos.y;
  y->pos.z += x->pos.z;
  y->pos.w += x->pos.w;
  y->rhw += x->rhw;
  y->tc.u += x->tc.u;
  y->tc.v += x->tc.v;
  y->color.r += x->color.r;
  y->color.g += x->color.g;
  y->color.b += x->color.b;
}

// �������������� 0-2 �����Σ����ҷ��غϷ����ε�����
int trapezoid_init_triangle(trapezoid_t* trap, const vertex_t* p1,
    const vertex_t* p2, const vertex_t* p3)
{
  const vertex_t* p;
  float k, x;

  if (p1->pos.y > p2->pos.y) {
    p = p1, p1 = p2, p2 = p;
  }

  if (p1->pos.y > p3->pos.y) {
    p = p1, p1 = p3, p3 = p;
  }

  if (p2->pos.y > p3->pos.y) {
    p = p2, p2 = p3, p3 = p;
  }

  if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y) {
    return 0;
  }

  if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x) {
    return 0;
  }

  if (p1->pos.y == p2->pos.y) { // triangle down
    if (p1->pos.x > p2->pos.x) {
      p = p1, p1 = p2, p2 = p;
    }

    trap[0].top = p1->pos.y;
    trap[0].bottom = p3->pos.y;
    trap[0].left.v1 = *p1;
    trap[0].left.v2 = *p3;
    trap[0].right.v1 = *p2;
    trap[0].right.v2 = *p3;
    return (trap[0].top < trap[0].bottom) ? 1 : 0;
  }

  if (p2->pos.y == p3->pos.y) { // triangle up
    if (p2->pos.x > p3->pos.x) {
      p = p2, p2 = p3, p3 = p;
    }

    trap[0].top = p1->pos.y;
    trap[0].bottom = p3->pos.y;
    trap[0].left.v1 = *p1;
    trap[0].left.v2 = *p2;
    trap[0].right.v1 = *p1;
    trap[0].right.v2 = *p3;
    return (trap[0].top < trap[0].bottom) ? 1 : 0;
  }

  trap[0].top = p1->pos.y;
  trap[0].bottom = p2->pos.y;
  trap[1].top = p2->pos.y;
  trap[1].bottom = p3->pos.y;

  k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
  x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

  if (x <= p3->pos.x) {   // triangle left
    trap[0].left.v1 = *p1;
    trap[0].left.v2 = *p2;
    trap[0].right.v1 = *p1;
    trap[0].right.v2 = *p3;
    trap[1].left.v1 = *p2;
    trap[1].left.v2 = *p3;
    trap[1].right.v1 = *p1;
    trap[1].right.v2 = *p3;
  }
  else {            // triangle right
    trap[0].left.v1 = *p1;
    trap[0].left.v2 = *p3;
    trap[0].right.v1 = *p1;
    trap[0].right.v2 = *p2;
    trap[1].left.v1 = *p1;
    trap[1].left.v2 = *p3;
    trap[1].right.v1 = *p2;
    trap[1].right.v2 = *p3;
  }

  return 2;
}

// ���� Y ��������������������������� Y �Ķ���
void trapezoid_edge_interp(trapezoid_t* trap, float y)
{
  float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
  float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
  float t1 = (y - trap->left.v1.pos.y) / s1;
  float t2 = (y - trap->right.v1.pos.y) / s2;
  vertex_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
  vertex_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);
}

// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ���
void trapezoid_init_scan_line(const trapezoid_t* trap, scanline_t* scanline, int y)
{
  float w = trap->right.v.pos.x - trap->left.v.pos.x;
  scanline->x = (int)(trap->left.v.pos.x + 0.5f);
  scanline->w = (int)(trap->right.v.pos.x + 0.5f) - scanline->x;
  scanline->y = y;
  scanline->v = trap->left.v;

  if (trap->left.v.pos.x >= trap->right.v.pos.x) {
    scanline->w = 0;
  }

  vertex_division(&scanline->step, &trap->left.v, &trap->right.v, w);
}

typedef struct device_t {
  float16 transform;          // ����任��
  int w;                  // ���ڿ��
  int h;                 // ���ڸ߶�
  uchar* framebuffer;         // ���ػ��棺framebuffer[y] ����� y��
  int* zbuffer;               // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��
  uchar* texture;             // ����ͬ����ÿ������
  int frame_pitch;
  int texture_pitch;
  int tex_w;              // ������
  int tex_h;              // ����߶�
  int max_u;                  // ��������ȣ�tex_w - 1
  int max_v;                  // �������߶ȣ�tex_h - 1
  int render_state;           // ��Ⱦ״̬
  COLOR background;           // ������ɫ
  COLOR foreground;           // �߿���ɫ
} device_t;

// ���õ�ǰ����
void device_set_texture(device_t* device, void* bits, long pitch, int w, int h)
{
  assert(w <= 1024 && h <= 1024);
  device->texture_pitch = pitch;
  device->texture = (uchar*)bits;
  device->tex_w = w;
  device->tex_h = h;
  device->max_u = (float)(w - 1);
  device->max_v = (float)(h - 1);
}
// ��� framebuffer �� zbuffer
void device_clear(device_t* device, int mode)
{
  int y, h = device->h;

  for (y = 0; y < device->h; y++) {
    uchar* dst = device->framebuffer + y * device->frame_pitch;
    UINT32 cc = (h - 1 - y) * 230 / (h - 1);
    cc = (cc << 16) | (cc << 8) | cc;

    if (mode == 0) {
      cc = device->background;
    }

    memset3(dst, cc, cc, cc, device->w);
  }

  memset4(device->zbuffer, 0, device->w * device->h);
}

#if 0
// ����
void device_pixel(device_t* device, int x, int y, UINT32 color)
{
  if (((UINT32)x) <= (UINT32)device->w && ((UINT32)y) <= (UINT32)device->h) {
    //device->framebuffer[y][x] = color;
  }
}
void imdraw_line24(img_t* im, int x1, int y1, int x2, int y2, COLOR c)
{
  uchar r = GetRV(c);
  uchar g = GetRV(c);
  uchar b = GetRV(c);
#define DEVICE_PIXEL(x1, y1, c)   DEVICE_PIXEL
#include "device_draw_line_impl.inl"
}
// �����߶�
void device_draw_line(device_t* device, int x1, int y1, int x2, int y2, UINT32 c)
{
  int x, y, rem = 0;
  //#include "device_draw_line_impl.inl"
}

// ���������ȡ����
UINT32 device_texture_read(const device_t* device, float u, float v)
{
  int x, y;
  u = u * device->max_u;
  v = v * device->max_v;
  x = (int)(u + 0.5f);
  y = (int)(v + 0.5f);
  x = CMID(x, 0, device->tex_w - 1);
  y = CMID(y, 0, device->tex_h - 1);
  return device->texture[y][x];
}


//=====================================================================
// ��Ⱦʵ��
//=====================================================================

// ����ɨ����
void device_draw_scanline(device_t* device, scanline_t* scanline)
{
  UINT32* framebuffer = device->framebuffer[scanline->y];
  float* zbuffer = device->zbuffer[scanline->y];
  int x = scanline->x;
  int w = scanline->w;
  int w = device->w;
  int render_state = device->render_state;

  for (; w > 0; x++, w--) {
    if (x >= 0 && x < w) {
      float rhw = scanline->v.rhw;

      if (rhw >= zbuffer[x]) {
        float w = 1.0f / rhw;
        zbuffer[x] = rhw;

        if (render_state & RENDER_STATE_COLOR) {
          float r = scanline->v.color.r * w;
          float g = scanline->v.color.g * w;
          float b = scanline->v.color.b * w;
          int R = (int)(r * 255.0f);
          int G = (int)(g * 255.0f);
          int B = (int)(b * 255.0f);
          R = CMID(R, 0, 255);
          G = CMID(G, 0, 255);
          B = CMID(B, 0, 255);
          framebuffer[x] = (R << 16) | (G << 8) | (B);
        }

        if (render_state & RENDER_STATE_TEXTURE) {
          float u = scanline->v.tc.u * w;
          float v = scanline->v.tc.v * w;
          UINT32 cc = device_texture_read(device, u, v);
          framebuffer[x] = cc;
        }
      }
    }

    vertex_add(&scanline->v, &scanline->step);

    if (x >= w) {
      break;
    }
  }
}

// ����Ⱦ����
void device_render_trap(device_t* device, trapezoid_t* trap)
{
  scanline_t scanline;
  int j, top, bottom;
  top = (int)(trap->top + 0.5f);
  bottom = (int)(trap->bottom + 0.5f);

  for (j = top; j < bottom; j++) {
    if (j >= 0 && j < device->h) {
      trapezoid_edge_interp(trap, (float)j + 0.5f);
      trapezoid_init_scan_line(trap, &scanline, j);
      device_draw_scanline(device, &scanline);
    }

    if (j >= device->h) {
      break;
    }
  }
}

// ���� render_state ����ԭʼ������
void device_draw_primitive(device_t* device, const vertex_t* v1,
    const vertex_t* v2, const vertex_t* v3)
{
  point_t p1, p2, p3, c1, c2, c3;
  int render_state = device->render_state;

  // ���� Transform �仯
  transform_apply(&device->transform, &c1, &v1->pos);
  transform_apply(&device->transform, &c2, &v2->pos);
  transform_apply(&device->transform, &c3, &v3->pos);

  // �ü���ע��˴���������Ϊ�����жϼ������� cvv���Լ�ͬcvv�ཻƽ����������
  // ���н�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ���� cvv�ڵ�������
  if (transform_check_cvv(&c1) != 0) {
    return;
  }

  if (transform_check_cvv(&c2) != 0) {
    return;
  }

  if (transform_check_cvv(&c3) != 0) {
    return;
  }

  // ��һ��
  transform_homogenize(&device->transform, &p1, &c1);
  transform_homogenize(&device->transform, &p2, &c2);
  transform_homogenize(&device->transform, &p3, &c3);

  // �������ɫ�ʻ���
  if (render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR)) {
    vertex_t t1 = *v1, t2 = *v2, t3 = *v3;
    trapezoid_t traps[2];
    int n;

    t1.pos = p1;
    t2.pos = p2;
    t3.pos = p3;
    t1.pos.w = c1.w;
    t2.pos.w = c2.w;
    t3.pos.w = c3.w;

    vertex_rhw_init(&t1); // ��ʼ�� w
    vertex_rhw_init(&t2); // ��ʼ�� w
    vertex_rhw_init(&t3); // ��ʼ�� w

    // ���������Ϊ0-2�����Σ����ҷ��ؿ�����������
    n = trapezoid_init_triangle(traps, &t1, &t2, &t3);

    if (n >= 1) {
      device_render_trap(device, &traps[0]);
    }

    if (n >= 2) {
      device_render_trap(device, &traps[1]);
    }
  }

  if (render_state & RENDER_STATE_WIREFRAME) {    // �߿����
    device_draw_line(device, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, device->foreground);
    device_draw_line(device, (int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, device->foreground);
    device_draw_line(device, (int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, device->foreground);
  }
}

//=====================================================================
// ������
//=====================================================================
vertex_t mesh[8] = {
  { {  1, -1,  1, 1 }, { 0, 0 }, { 1.0f, 0.2f, 0.2f }, 1 },
  { { -1, -1,  1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 0.2f }, 1 },
  { { -1,  1,  1, 1 }, { 1, 1 }, { 0.2f, 0.2f, 1.0f }, 1 },
  { {  1,  1,  1, 1 }, { 1, 0 }, { 1.0f, 0.2f, 1.0f }, 1 },
  { {  1, -1, -1, 1 }, { 0, 0 }, { 1.0f, 1.0f, 0.2f }, 1 },
  { { -1, -1, -1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 1.0f }, 1 },
  { { -1,  1, -1, 1 }, { 1, 1 }, { 1.0f, 0.3f, 0.3f }, 1 },
  { {  1,  1, -1, 1 }, { 1, 0 }, { 0.2f, 1.0f, 0.3f }, 1 },
};

void draw_plane(device_t* device, int a, int b, int c, int d)
{
  vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
  p1.tc.u = 0, p1.tc.v = 0, p2.tc.u = 0, p2.tc.v = 1;
  p3.tc.u = 1, p3.tc.v = 1, p4.tc.u = 1, p4.tc.v = 0;
  device_draw_primitive(device, &p1, &p2, &p3);
  device_draw_primitive(device, &p3, &p4, &p1);
}

void draw_box(device_t* device, float theta)
{
  matrix_t m;
  matrix_set_rotate(&m, -1, -0.5, 1, theta);
  device->transform.world = m;
  transform_update(&device->transform);
  draw_plane(device, 0, 1, 2, 3);
  draw_plane(device, 4, 5, 6, 7);
  draw_plane(device, 0, 4, 5, 1);
  draw_plane(device, 1, 5, 6, 2);
  draw_plane(device, 2, 6, 7, 3);
  draw_plane(device, 3, 7, 4, 0);
}

void camera_at_zero(device_t* device, float x, float y, float z)
{
  point_t eye = { x, y, z, 1 }, at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
  matrix_set_lookat(&device->transform.view, &eye, &at, &up);
  transform_update(&device->transform);
}

#endif
