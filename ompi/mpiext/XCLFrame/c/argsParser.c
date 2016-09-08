

#include "c_Interface.h"

static int autoTune_flag;
int parseArguments(int argc, char ** argv,	char* heuristicModel,
char* selectedDevices){
	int c;
	int configInputs=0;


	while (1)
	{
		static struct option long_options[] =
		{
				/* These options don’t set a flag.
	             We distinguish them by their indices. */
				{"taskfile",   required_argument, 0, 't'},
				{"static scheduling",  optional_argument, 0, 's'},
				{"dynamic scheduling", optional_argument, 0, 'y'},
				{"autotune",   no_argument, &autoTune_flag, 1},
				{"devicetype", required_argument, 0, 'd'},
				//{"nT", required_argument, 0, 'n'},
				{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "ad:t:s::y::",
				long_options, &option_index);

		// Detect the end of the options.
		if (c == -1)
			break; //exit loop.

		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 't':
			printf ("option -t with value `%s'\n", optarg);
			configInputs+=TASKFILE;
			break;

		case 's':
			printf ("option -s with value `%s'\n", optarg);
			configInputs+=STATICSCHED;
			if(optarg){
				memcpy(heuristicModel,optarg,sizeof(optarg));
			}else{
				memcpy(heuristicModel,DEFAULT_STATIC,sizeof(DEFAULT_STATIC));
			}
			break;

		case 'y':
			printf ("option -y with value `%s'\n", optarg);
			configInputs+=DYNAMICSCHED;
			if(optarg){
				memcpy(heuristicModel,optarg,sizeof(optarg));
			}else{
				memcpy(heuristicModel,DEFAULT_DYNAMIC,sizeof(DEFAULT_DYNAMIC));
			}
			break;

		case 'd':
			printf ("option -d with value `%s'\n", optarg);
			configInputs+=DEVICEFILTER;
			memcpy(selectedDevices,optarg,sizeof(optarg));
			break;

		/*case 'n':
			printf ("option -n with value `%s'\n", optarg);
			configInputs+=NUMTASKS;
			numTasks=atoi(optarg);
			break;*/

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort ();
		}//end switch
	} //end of options loop

	if(autoTune_flag){
		configInputs+=AUTOTUNE;
	}


	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		printf (" \n non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
	}


	return configInputs;
}
