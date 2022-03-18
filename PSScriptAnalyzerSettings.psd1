@{
    'ExcludeRules' = @(
        'PSAvoidUsingPositionalParameters',
        'PSAvoidUsingWriteHost',
        'PSUseApprovedVerbs',
        'PSUseShouldProcessForStateChangingFunctions'
    )

    'Rules' = @{
        'PSAvoidUsingCmdletAliases' = @{
            'Whitelist' = @('echo', 'foreach', 'ls', 'measure', 'rm', 'select', 'sleep', 'where')
        }
    }
}
