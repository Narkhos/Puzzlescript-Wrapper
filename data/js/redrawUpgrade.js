var old_redraw = redraw;
redraw = function() {
	nativeClearInstances();
	old_redraw();
}

consolePrint = console.log;

// Activate verbose_logging for debugging
verbose_logging = true;