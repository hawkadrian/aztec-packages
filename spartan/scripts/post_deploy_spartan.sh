#!/usr/bin/env bash
# Targets a running cluster and deploys example contracts for testing
set -eu
set -o pipefail

echo "Bootstrapping network with test contracts"

export NAMESPACE=${1:-spartan}
TAG=${2:-latest}
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -z "$NAMESPACE" ]; then
  echo "Usage: $0 (optional: <namespace>)"
  echo "Example: $0 devnet"
  exit 1
fi

# Helper function to get load balancer URL based on namespace and service name
function get_load_balancer_url() {
  local namespace=$1
  local service_name=$2
  kubectl get svc -n $namespace -o jsonpath="{.items[?(@.metadata.name=='$service_name')].status.loadBalancer.ingress[0].hostname}"
}

# Fetch the service URLs based on the namespace for injection in the test-transfer.sh
export BOOTNODE_URL=$($(dirname $0)/get_service_address boot-node 8080)
export PXE_URL=$($(dirname $0)/get_service_address pxe 8080)
export ETHEREUM_HOSTS=$($(dirname $0)/get_service_address eth-execution 8545)

echo "BOOTNODE_URL: $BOOTNODE_URL"
echo "PXE_URL: $PXE_URL"
echo "ETHEREUM_HOSTS: $ETHEREUM_HOSTS"

echo "Bootstrapping contracts for test network. NOTE: This took one hour last run."

docker run aztecprotocol/aztec:$TAG bootstrap-network \
  --rpc-url $BOOTNODE_URL \
  --l1-rpc-urls $ETHEREUM_HOSTS \
  --l1-chain-id 1337 \
  --l1-private-key 0xac0974bec39a17e36ba4a6b4d238ff944bacb478cbed5efcae784d7bf4f2ff80 \
  --json | tee ./basic_contracts.json
