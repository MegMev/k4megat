def init_analysis(mainparser):
    from mgana.ana_builder import setup_analysis

    args, _ = mainparser.parse_known_args()
    if args.command == 'init':
        setup_analysis(package=args.package,
                       name=args.name,
                       output_dir=args.output_dir)
