Template: hotplug/static_module_list
Type: multiselect
Choices: ${usbmodules}
Description: Which USB modules do you want to be preloaded?
 Before running hotplug support, you can preload some USB modules. 
Description-es: �Qu� m�dulos USB desea precargar?
 Antes de ejecutar hotplug, puede precargar algunos m�dulos USB.

Template: hotplug/usb_keyboard
Type: note
Description: USB keyboard configuration
 If you're booting with a USB keyboard and/or mouse, and want to defend
 against boot failures like missing modules, you should probably use static
 linking for the "hid", "keybdev" (and/or "mousedev"), "input", "usbcore",
 and USB Host Controller modules.  
Description-es: Configuraci�n del teclado USB
 Si arranca con un teclado y/o rat�n USB y quiere evitar errores de
 arranque debidos a la falta de alg�n m�dulo, probablemente deber�a
 enlazar est�ticamente "hid", "keybdev" (y/o "mousedev"), "input",
 "usbcore" y los m�dulos USB Host Controller.

Template: hotplug/x11_usbmice_hack
Type: boolean
Default: false
Description: Do you use USB mouse with X11?
 If you need to use a USB mouse with X11, the X server needs to be able to
 open "/dev/input/mice".  That means you can't hotplug a mouse you plan to
 use with X11; it needs some modules preloaded. If you haven't compiled
 them into your kernel, Answer Yes. (X11_USBMICE_HACK=true) 
Description-es: �Usa un rat�n USB en X11?
 Si necesita usar un rat�n USB en X11, el servidor X necesita poder abrir
 "/dev/input/mice". Esto significa que no puede enchufar en caliente un
 rat�n para usarlo con X11; se necesitan algunos m�dulos ya cargados. Si
 no los ha compilado en el n�cleo, responda S�. (X11_USBMICE_HACK=true)

