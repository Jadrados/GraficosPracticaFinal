"""
obj_to_header.py
Convierte un archivo .obj (con v/vt/vn y caras quad o tri) en un .h
compatible con el formato de BOX.h del proyecto IGLib.
Uso: python obj_to_header.py <archivo.obj> <NombreModelo>
"""

import sys
import os
import math

def parse_obj(filepath):
    positions = []
    normals = []
    texcoords = []
    face_refs = []  # lista de listas de (vi, ti, ni) por cara

    with open(filepath, encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.strip()
            if line.startswith('v '):
                vals = line.split()[1:]
                positions.append([float(x) for x in vals])
            elif line.startswith('vn '):
                vals = line.split()[1:]
                normals.append([float(x) for x in vals])
            elif line.startswith('vt '):
                vals = line.split()[1:]
                texcoords.append([float(x) for x in vals[:2]])
            elif line.startswith('f '):
                verts = line.split()[1:]
                face = []
                for v in verts:
                    parts = v.split('/')
                    vi = int(parts[0]) - 1
                    ti = int(parts[1]) - 1 if len(parts) > 1 and parts[1] != '' else -1
                    ni = int(parts[2]) - 1 if len(parts) > 2 and parts[2] != '' else -1
                    face.append((vi, ti, ni))
                face_refs.append(face)

    return positions, normals, texcoords, face_refs


def triangulate(faces):
    """Convierte quads (y polígonos) en triángulos usando fan triangulation."""
    tris = []
    for face in faces:
        for i in range(1, len(face) - 1):
            tris.append((face[0], face[i], face[i+1]))
    return tris


def compute_tangent(p0, p1, p2, uv0, uv1, uv2):
    """Calcula el tangente usando las UVs."""
    e1 = [p1[i] - p0[i] for i in range(3)]
    e2 = [p2[i] - p0[i] for i in range(3)]
    du1 = uv1[0] - uv0[0]
    dv1 = uv1[1] - uv0[1]
    du2 = uv2[0] - uv0[0]
    dv2 = uv2[1] - uv0[1]
    denom = du1 * dv2 - du2 * dv1
    if abs(denom) < 1e-8:
        return [1.0, 0.0, 0.0]
    f = 1.0 / denom
    t = [f * (dv2 * e1[i] - dv1 * e2[i]) for i in range(3)]
    length = math.sqrt(sum(x*x for x in t))
    if length < 1e-8:
        return [1.0, 0.0, 0.0]
    return [x / length for x in t]


def mushroom_color(y, y_min, y_max):
    """Color basado en altura: cap = rojo/naranja, stem = blanco/beige."""
    t = (y - y_min) / (y_max - y_min) if y_max != y_min else 0.5
    if t > 0.45:
        # Cap: rojo-naranja
        return [0.85, 0.25, 0.10]
    else:
        # Stem: blanco-beige
        return [0.95, 0.90, 0.80]


def generate_header(obj_path, model_name):
    positions, normals, texcoords, face_refs = parse_obj(obj_path)
    triangles = triangulate(face_refs)

    # Calcular bounding box para escalar
    all_y = [p[1] for p in positions]
    y_min = min(all_y)
    y_max = max(all_y)
    all_coords = [c for p in positions for c in p]
    obj_min = min(all_coords)
    obj_max = max(all_coords)
    extent = obj_max - obj_min if obj_max != obj_min else 1.0
    scale = 2.0 / extent  # escalar para que quepa en un cubo de ~2 unidades

    # Offset para centrar el modelo en Y
    y_center = (y_min + y_max) / 2.0

    # Expandir vértices (un vértice por referencia en cada triángulo)
    out_pos = []
    out_nor = []
    out_tex = []
    out_tan = []
    out_col = []
    out_idx = []

    idx = 0
    for tri in triangles:
        tri_verts = []
        for (vi, ti, ni) in tri:
            p = positions[vi]
            px = p[0] * scale
            py = (p[1] - y_center) * scale
            pz = p[2] * scale
            out_pos.extend([px, py, pz])

            if ni >= 0 and ni < len(normals):
                out_nor.extend(normals[ni])
            else:
                out_nor.extend([0.0, 1.0, 0.0])

            if ti >= 0 and ti < len(texcoords):
                out_tex.extend(texcoords[ti])
            else:
                out_tex.extend([0.0, 0.0])

            col = mushroom_color(p[1], y_min, y_max)
            out_col.extend(col)

            tri_verts.append(((positions[vi][0], positions[vi][1], positions[vi][2]),
                              texcoords[ti] if (ti >= 0 and ti < len(texcoords)) else [0.0, 0.0]))

        # Calcular tangente para los 3 vértices del triángulo
        p0, uv0 = tri_verts[0]
        p1, uv1 = tri_verts[1]
        p2, uv2 = tri_verts[2]
        tan = compute_tangent(list(p0), list(p1), list(p2), list(uv0), list(uv1), list(uv2))
        out_tan.extend(tan)
        out_tan.extend(tan)
        out_tan.extend(tan)

        out_idx.extend([idx, idx+1, idx+2])
        idx += 3

    n_vertex = len(out_pos) // 3
    n_triangles = len(triangles)

    lines = []
    lines.append(f'#ifndef __{model_name.upper()}_FILE__')
    lines.append(f'#define __{model_name.upper()}_FILE__')
    lines.append('')
    lines.append(f'// Modelo generado desde: {os.path.basename(obj_path)}')
    lines.append(f'// Vertices: {n_vertex}  Triangulos: {n_triangles}')
    lines.append('')
    lines.append(f'const int {model_name}NVertex = {n_vertex};')
    lines.append(f'const int {model_name}NTriangleIndex = {n_triangles};')
    lines.append('')

    # Indices
    lines.append(f'const unsigned int {model_name}TriangleIndex[] = {{')
    for i in range(0, len(out_idx), 3):
        lines.append(f'\t{out_idx[i]}, {out_idx[i+1]}, {out_idx[i+2]},')
    lines.append('};')
    lines.append('')

    def fmt3(arr, per_vertex=3):
        result = []
        for i in range(0, len(arr), per_vertex):
            vals = ', '.join(f'{v:.6f}f' for v in arr[i:i+per_vertex])
            result.append(f'\t{vals},')
        return result

    # Posiciones
    lines.append(f'const float {model_name}VertexPos[] = {{')
    lines.extend(fmt3(out_pos, 3))
    lines.append('};')
    lines.append('')

    # Colores
    lines.append(f'const float {model_name}VertexColor[] = {{')
    lines.extend(fmt3(out_col, 3))
    lines.append('};')
    lines.append('')

    # Normales
    lines.append(f'const float {model_name}VertexNormal[] = {{')
    lines.extend(fmt3(out_nor, 3))
    lines.append('};')
    lines.append('')

    # UVs
    lines.append(f'const float {model_name}VertexTexCoord[] = {{')
    lines.extend(fmt3(out_tex, 2))
    lines.append('};')
    lines.append('')

    # Tangentes
    lines.append(f'const float {model_name}VertexTangent[] = {{')
    lines.extend(fmt3(out_tan, 3))
    lines.append('};')
    lines.append('')
    lines.append(f'#endif')
    lines.append('')

    return '\n'.join(lines)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Uso: python obj_to_header.py <archivo.obj> <NombreModelo>')
        sys.exit(1)
    obj_path = sys.argv[1]
    model_name = sys.argv[2]
    output = generate_header(obj_path, model_name)
    out_file = os.path.splitext(obj_path)[0] + '.h'
    with open(out_file, 'w', encoding='utf-8') as f:
        f.write(output)
    print(f'Generado: {out_file}')
    print(f'  Vertices: {output.count("VertexPos")}')
