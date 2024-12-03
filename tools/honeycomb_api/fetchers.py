findDelegateAuthority = """
query DelegateAuthority(
  $addresses: [Bytes!],
  $delegates: [Pubkey!],
  $projects: [Pubkey!]
) {
  delegateAuthority(
    addresses: $addresses,
    delegates: $delegates,
    projects: $projects
  ) {
    address
    bump
    project
    authority
    delegations {
      kind
      index
      permission
    }
  }
}
"""