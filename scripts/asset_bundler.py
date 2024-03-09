import os
import json
import tempfile
import shutil
import click
from pathlib import Path
from gltflib import GLTF, FileResource
from os import path


def process_gltf(file_set, source_directory, target_directory):
    with tempfile.TemporaryDirectory() as temp_directory:
        for gltf_file in file_set:
            source_file = path.join(source_directory, gltf_file)
            target_file = path.join(target_directory, gltf_file)
            staging_file = path.join(temp_directory, gltf_file)

            click.echo(f"process_gltf:")
            click.echo(f"\tSRC: {path.relpath(source_file)}")
            click.echo(f"\tTMP: {path.abspath(staging_file)}")
            click.echo(f"\tTRG: {path.relpath(target_file)}")

            source_gltf = GLTF.load(source_file)
            source_gltf.export_gltf(staging_file, save_file_resources=True)

            staging_gltf = GLTF.load(staging_file)
            exclude_uris = []
            staging_resources = []

            # Rename every image and exclude their uris from copy, create a new resource for each image.
            for index, image in enumerate(staging_gltf.model.images):
                # TODO(cedmundo): Get image format from the texture within the model.
                # TODO(cedmundo): PNGs should use srgb but JPGs and other formats don't need that.
                # TODO(cedmundo): PNGs need primaries converted but JPGs maybe not.
                original_image_path = path.join(temp_directory, image.uri)
                dest_uri = str(Path(image.uri).with_suffix(".ktx"))
                dest_image_path = path.join(temp_directory, dest_uri)
                os.system(f"""ktx create --format R8G8B8A8_SRGB\
                    --assign-oetf srgb\
                    --assign-primaries srgb\
                    {original_image_path} {dest_image_path}""")

                # Do not copy following resource
                dest_resource = FileResource(filename=dest_uri, basepath=temp_directory)
                dest_resource.export(target_directory)
                staging_resources.append(dest_resource)
                exclude_uris.append(image.uri)
                
                image.uri = dest_resource.uri
                image.mimeType = dest_resource.mimetype
                image.index = index

                for texture in staging_gltf.model.textures:
                    if texture.source == index:
                        texture.source = None
                        texture.extensions = {"KHR_texture_basisu": {"source": index}}

            # Add remaining .bin and not processed resources as they were
            for resource in staging_gltf.resources:
                if resource.uri not in exclude_uris:
                    staging_resources.append(resource)

            target_gltf = GLTF(staging_gltf.model, resources=staging_resources)
            target_gltf.model.extensionsUsed = ["KHR_texture_basisu"]
            target_gltf.model.extensionsRequired = ["KHR_texture_basisu"]
            target_gltf.export_gltf(target_file, save_file_resources=True)


def copy_files(file_set, source_directory, target_directory):
    for copying_file in file_set:
        source_file = path.join(source_directory, copying_file)
        target_file = path.join(target_directory, copying_file)
        click.echo(f"copy_files:")
        click.echo(f"\tSRC: {path.relpath(source_file)}")
        click.echo(f"\tTRG: {path.relpath(target_file)}")
        shutil.copyfile(source_file, target_file)


@click.command()
@click.option('--manifest', default='../assets/manifest.json', help='Location of the manifest file')
@click.option('--target', default='assets', help='Target directory location (must exist)')
def process_manifest(manifest, target):
    with open(manifest, "r") as manifest_file:
        manifest_content = manifest_file.read()
        manifest_data = json.loads(manifest_content)
        source_directory = path.dirname(path.abspath(manifest))
        target_directory = path.abspath(target)
        process_gltf(manifest_data.get("process_gltf", []), source_directory, target_directory)
        copy_files(manifest_data.get("copy_files", []), source_directory, target_directory)


if __name__ == '__main__':
    process_manifest()
