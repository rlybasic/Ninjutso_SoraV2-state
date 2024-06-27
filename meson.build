project('Ninjutso_SoraV2_state', 'c',
        default_options: ['c_std=c11']
)

# Fetch HIDAPI subproject
hidapi_proj = subproject('hidapi')
hidapi_dep = hidapi_proj.get_variable('hidapi_dep')

executable('Ninjutso_SoraV2_state', 'main.c',
           dependencies: [hidapi_dep]
)
