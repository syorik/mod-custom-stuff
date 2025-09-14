// From your module sources
void AddCustomStuffScripts();

// Export name must be: Add + <module_folder_name> + Scripts
// Folder: mod-custom-stuff  ->  Addmod_custom_stuffScripts
void Addmod_custom_stuffScripts()
{
    AddCustomStuffScripts();
}
