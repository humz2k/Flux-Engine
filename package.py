import os
import sys
import shutil

def copy_files(src_dir, dest_dir):
    assets = os.listdir(src_dir)
    for src,dest in zip([os.path.join(src_dir,i) for i in assets],[os.path.join(dest_dir,i) for i in assets]):
        shutil.copy(src,dest)

def copy_project_dir(build_dir):
    project_dir = os.path.join(build_dir,"project")
    os.mkdir(project_dir)

    assets_dir = os.path.join(project_dir,"assets")
    os.mkdir(assets_dir)

    copy_files("project/assets", assets_dir)

    prefabs_dir = os.path.join(project_dir,"prefabs")
    os.mkdir(prefabs_dir)

    copy_files("project/prefabs", prefabs_dir)

    scenes_dir = os.path.join(project_dir,"scenes")
    os.mkdir(scenes_dir)

    copy_files("project/scenes", scenes_dir)

def copy_src(build_dir):
    src_dir = os.path.join(build_dir,"src")
    os.mkdir(src_dir)

    renderer_dir = os.path.join(src_dir,"renderer")
    os.mkdir(renderer_dir)
    shaders_dir = os.path.join(renderer_dir,"shaders")
    os.mkdir(shaders_dir)

    copy_files("src/renderer/shaders",shaders_dir)

    editor_dir = os.path.join(src_dir,"editor")
    os.mkdir(editor_dir)

    shutil.copy("src/editor/JetBrainsMono-Regular.ttf",os.path.join(editor_dir,"JetBrainsMono-Regular.ttf"))

def package(name):

    os.system('make clean')
    os.system('make PACKAGE=true -j')

    build_dir = os.path.join("build",name)
    os.system('rm -rf ' + build_dir)
    os.mkdir(build_dir)

    copy_project_dir(build_dir)
    copy_src(build_dir)

    shutil.move('build/driver',os.path.join(build_dir,'driver'))

if __name__ == "__main__":
    package(sys.argv[1])