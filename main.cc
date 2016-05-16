
#include "main.h"

int main(int argc, char* argv[]){

	//Init
	fileList = NULL;
	currentMode = COMPRESS;

	if(argc > 1){
		parseCommandLine(argc, argv);
	}
	else{
		initGUI(argc, argv);
	}

	return 0;
}

void initGUI(int argc, char* argv[]){
		gtk_init(&argc, &argv);

		//Load gui
		builder = gtk_builder_new();
		gtk_builder_add_from_file(builder,"gui/HuffmanCodecUI.glade", NULL);
		gtk_builder_connect_signals(builder, NULL);

		//Get widget
		mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
		fileListModel = GTK_LIST_STORE(gtk_builder_get_object(builder, "fileListModel"));

		assert(mainWindow != NULL);
		assert(fileListModel != NULL);

		//Disable compress, decompress and remove button
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "compToolButton")), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "removeToolButton")), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "decToolButton")), FALSE);

		gtk_main();
}

void parseCommandLine(int argc, char* argv[]){
	gboolean useCompress = FALSE;
	gboolean useDecompress = FALSE;
	gchar* inPath = NULL;
	gchar* outPath = NULL;
	int c;

	//Parse all the options
	while((c = getopt(argc, argv, "c:d:ho:")) != -1){
		switch (c) {
			//Compress -c [FILE1] [FILE2] ... [FILEn]
			case 'c':
				if(useDecompress){
					cout << "Cannot use -c with option -d" << endl;
					exit(1);
				}else{
					useCompress = TRUE;
					if(!parseArgFileList(argc, argv)){
						cout << "Some file in the list don't exist" << endl;
					}
				}
				break;

			//Decompress -d [FILE]
			case 'd':
				if(useCompress){
					cout << "Cannot use -d with option -c" << endl;
					exit(1);
				}else{
					useDecompress = TRUE;
					inPath = optarg;
				}
				break;

			//Help -h
			case 'h':
				cout << "\t-c [FILE]...\t to compress. The first file is archive output file name." << endl;
				cout << "\t-d [FILE]   \t to decompress." << endl;
				cout << "\t-o [FILE] | [DIR]   \t to set the output  [FILE] | [DIR]." << endl;
				break;

			//Set output file/dir 	-o [FILE] | [DIR]
			case 'o':
				outPath = optarg;
				break;

			//Error
			case '?':
			default:
				exit(1);
		}
	}

	//Compress
	if(useCompress){
		if(outPath != NULL){
			if(!compressToFile(outPath, fileList)){
				cout << "Error during the compression" << endl;
				exit(1);
			}
		}
		else{
			cout << "No output dir setted. Use -o to set." << endl;
			exit(1);
		}
	//Decompress
	}else if(useDecompress){
		if(outPath != NULL){

			if(!getCompressedFileInfo(inPath, fileList)){
				cout << "Error opening the archive file" << endl;
				exit(1);
			}

			if(!decompressFileToDir(inPath, outPath, fileList)){
				cout << "Error during the decompression" << endl;
				exit(1);
			}
		}
		else{
			cout << "No input file name setted. Use -o to set." << endl;
			exit(1);
		}
	}
}

gboolean parseArgFileList(int argc, char* argv[]){
	gchar* path;
	int index = optind - 1;
	FileInfo* info;

	while(index < argc){
		 path = strdup(argv[index]);
		 index++;
		 if(path[0] != '-'){
				 info = createNewFileInfo(path);
				 if(info != NULL){
					 	//If the file is not already in the fileList
				 		if(g_slist_find_custom(fileList, g_path_get_basename(path), findFileByName) == NULL){
							fileList = g_slist_append(fileList, info);
						}
				 }
				 else{
					 return FALSE;
				 }
		 }
		 else{
			 optind = index - 1;
			 return TRUE;
		}
	}
	return TRUE;
}

GtkWidget* createFileChooser(GtkWindow* parent, GtkFileChooserAction action, const gchar* title){
	//Prepare file open dialog
	GtkWidget* dialog = gtk_file_chooser_dialog_new (
																			title,
																			parent,
																			action,
																			"Cancel",
																			GTK_RESPONSE_CANCEL,
																			"OK",
																			GTK_RESPONSE_ACCEPT,
																			NULL);
	GdkGeometry geom;
	geom.min_width = 800;
	geom.min_height = 600;

	//Resize the window
	gtk_window_set_geometry_hints(GTK_WINDOW(dialog), NULL, &geom, GDK_HINT_MIN_SIZE);

	return dialog;
}

void showMessageDialog(GtkWindow* parent, GtkMessageType type, const gchar* msg){
	GtkWidget* dialog = gtk_message_dialog_new(
														parent,
														GTK_DIALOG_MODAL,
														type,
														GTK_BUTTONS_OK,
														"%s", msg);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void addToFileList(const gchar* path){
	//fileInfo pointer
	FileInfo* fileInfo;

	//If the file is not already in the fileList
	if(g_slist_find_custom(fileList, g_path_get_basename(path), findFileByName) == NULL){
		//Create the fileInfo
		fileInfo = createNewFileInfo(path);

		if(fileInfo != NULL){
			//Add the fileInfo to the fileList
			fileList = g_slist_append(fileList, fileInfo);
			//Add the file to the view
			addToFileView(fileInfo);
		}
	}
}

void addToFileView(FileInfo* info){
	//Get file list model
	GtkTreeIter iter;

	//Calculate compress rateo
	int rateo = getFileCompressedSize(info) == 0 ? 0 : (float)(getFileSize(info) - getFileCompressedSize(info)) / getFileSize(info) * 100;

	//Append the file to the fileListModel
	gtk_list_store_append(fileListModel, &iter);
	gtk_list_store_set (fileListModel, &iter,
						COL_NAME, getFileName(info),
						COL_TYPE, getFileFormat(info),
						COL_SIZE, getFileSize(info),
						COL_CMP_SIZE, getFileCompressedSize(info),
						COL_CMP_RATEO, rateo,
						-1);
}

void clearFileView(){
	gtk_list_store_clear(fileListModel);
}

void clearFileList(){
	if(fileList != NULL){
		g_slist_free_full(fileList, deleteFileInfo);
		fileList = NULL;
	}
}

void reloadFileView(){
	GSList* current = fileList;

	clearFileView();
	while(current != NULL){
		addToFileView((FileInfo*)current->data);
		current = g_slist_next(current);
	}
}

int getFileViewSelectedRow(){
	GtkTreeView* fileListView = GTK_TREE_VIEW(gtk_builder_get_object(builder, "fileListView"));
	GtkTreeSelection* selection = gtk_tree_view_get_selection(fileListView);
	GtkTreeModel* model;
	GtkTreePath* path;
	GtkTreeIter iter;
	gint* i;

	if (gtk_tree_selection_get_selected(selection , &model , &iter)){
	  path = gtk_tree_model_get_path(model , &iter) ;
	  i = gtk_tree_path_get_indices(path);
	  return (*i);
	}

	return -1;
}

//Signals handler

//Open file to decompress
GTK_HANDLER(void onOpenClick){
	//Create file chooser dialog
	GtkWidget* dialog = createFileChooser(GTK_WINDOW(mainWindow), GTK_FILE_CHOOSER_ACTION_OPEN, "Open file");

	//Set file chooser filter
	GtkFileFilter* filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.hca");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	//Exec the dialog
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	//If ok button is pressed
	if (res == GTK_RESPONSE_ACCEPT){
		//Change currentMode
		currentMode = DECOMPRESS;

		//Disable compress and remove button
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "compToolButton")), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "removeToolButton")), FALSE);

		//Remove all loaded files
		clearFileList();

		//Get the file to decompress path
	 	fileToDecompress = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		//Parse the file header
		if(getCompressedFileInfo(fileToDecompress, fileList)){
			//If the parse is correct enable the decompress button
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "decToolButton")), TRUE);
			//Reload the parsed file header
			reloadFileView();
		}
		else{
			showMessageDialog(GTK_WINDOW(mainWindow), GTK_MESSAGE_ERROR, "Invalid file header");
		}
	}
	gtk_widget_destroy(dialog);
}

//Decompress the selected file
GTK_HANDLER(void onDecompressClick){
	//Create file chooser dialog
	GtkWidget* fileChooser = createFileChooser(GTK_WINDOW(mainWindow), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Select a directory");
	gtk_file_chooser_set_create_folders (GTK_FILE_CHOOSER(fileChooser), TRUE);

	//Exec the dialog
	gint res = gtk_dialog_run(GTK_DIALOG(fileChooser));

	//Get the save path
	gchar* decompressPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
	gtk_widget_destroy(fileChooser);

	//If ok button is pressed
	if (res == GTK_RESPONSE_ACCEPT){

		if(!decompressFileToDir(fileToDecompress, decompressPath, fileList)){
			showMessageDialog(GTK_WINDOW(mainWindow), GTK_MESSAGE_ERROR, "Error during the decompression");
		}

		g_free(decompressPath);
	}

}

//Add file to compress
GTK_HANDLER(void onAddClick){
	//Create file chooser dialog
	GtkWidget* dialog = createFileChooser(GTK_WINDOW(mainWindow), GTK_FILE_CHOOSER_ACTION_OPEN, "Open file");

	//Allow to select more than one file from the dialog
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

	//Set file chooser filter
	GtkFileFilter* filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.txt");
	gtk_file_filter_add_pattern(filter, "*.c");
	gtk_file_filter_add_pattern(filter, "*.cc");
	gtk_file_filter_add_pattern(filter, "*.h");
	gtk_file_filter_add_pattern(filter, "*.html");
	gtk_file_filter_add_pattern(filter, "*.xml");
	gtk_file_filter_add_pattern(filter, "*.doc");
	gtk_file_filter_add_pattern(filter, "*.docx");
	gtk_file_filter_add_pattern(filter, "*.odt");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

	//Exec the dialog
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	//If ok button is pressed
	if (res == GTK_RESPONSE_ACCEPT){
			if(currentMode != COMPRESS){
				clearFileList();
				clearFileView();
			}

			currentMode = COMPRESS;
			//Get all selected files list
	    GSList* filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
			GSList* current = filenames;
			gchar* path;

			//Iter on each selected file
			while(current != NULL){
				path = (gchar*)current->data;
				addToFileList(path);
				//Go to the next file
				current = g_slist_next(current);
			}

			//If there is at least one valid file
			if(fileList != NULL){
				//Enable compress and remove button and disable decompress
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "compToolButton")), TRUE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "decToolButton")), FALSE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "removeToolButton")), TRUE);
			}

			//Free list
			g_slist_free(filenames);
	}
	//Free dialog
	gtk_widget_destroy(dialog);
}

//Remove file from compress list
GTK_HANDLER(void onRemoveClick){
	int index = getFileViewSelectedRow();
	GSList* fileInfoNode;
	GtkTreeIter iter;

	//If a row is selected
	if(index != -1 && currentMode == COMPRESS){
		if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(fileListModel), &iter, NULL, index)){
			//Get the n-th node
			fileInfoNode = g_slist_nth(fileList, index);
			//Remove the node from the list and update the head
			fileList = g_slist_remove_link(fileList, fileInfoNode);
			//Delete the node data
			deleteFileInfo((FileInfo*)fileInfoNode->data);
			//Delete the node
			g_slist_free_1(fileInfoNode);
			//Remove the n-th row
			gtk_list_store_remove(fileListModel, &iter);

			//If file list is empty -> disable compress and remove button
			if(fileList == NULL){
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "compToolButton")), FALSE);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder, "removeToolButton")), FALSE);
			}

		}
	}
}

//Compress the selected files
GTK_HANDLER(void onCompressClick){
	//If there is at least a file
	if(fileList != NULL){
		gchar* outputFilePath;
		//Create the file chooser
		GtkWidget* fileChooser = createFileChooser(GTK_WINDOW(mainWindow), GTK_FILE_CHOOSER_ACTION_SAVE, "Save on file");

		//Set the file format filter
		GtkFileFilter* filter = gtk_file_filter_new();
		gtk_file_filter_add_pattern(filter, "*.hca");
		gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(fileChooser), filter);

		gint res = gtk_dialog_run(GTK_DIALOG(fileChooser));

		//Get the save path
		outputFilePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
		gtk_widget_destroy(fileChooser);

		//If dialog returned OK
		if(res == GTK_RESPONSE_ACCEPT){
			//Add extension if missing
			if(!g_str_has_suffix(outputFilePath, ".hca")){
				outputFilePath = g_strconcat(outputFilePath, ".hca", NULL);
			}

			//Try to compress the files
			if(!compressToFile(outputFilePath, fileList)){
				showMessageDialog(GTK_WINDOW(mainWindow), GTK_MESSAGE_ERROR, "Error during the compression");
			}
			else{
				reloadFileView();
			}
		}
		g_free(outputFilePath);
	}
	else{
		showMessageDialog(GTK_WINDOW(mainWindow), GTK_MESSAGE_ERROR, "No file to compress");
	}
}

GTK_HANDLER(void onInfoClicked){
	GtkWidget* aboutDialog = GTK_WIDGET(gtk_builder_get_object(builder, "aboutDialog"));
	gtk_widget_show_all(aboutDialog);
}

//Quit signal
GTK_HANDLER(gboolean onWindowDelete){
		clearFileList();
    gtk_main_quit() ;
    return TRUE ;
}

GTK_HANDLER(gboolean onInfoClosed){
    gtk_widget_hide(widget);
    return TRUE;
}
