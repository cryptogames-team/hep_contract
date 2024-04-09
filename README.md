hep.contracts (by Cryptogames)
=============
EOSIO 기반 블록체인에서 블록 생산자 등록 및 투표, CPU 및 네트워크 대역폭에 대한 토큰 스테이킹, RAM 구매, 다중 서명 등의 기능을 실행하려면 여러 스마트 컨트랙트가 배포되어야합니다. 이러한 스마트 계약에는 bios, system, msig가 존재합니다.

CryptoGames는 기존 EOSIO의 블록 생산자의 페널티와 보상 지급방식을 변경하였습니다. 이에 해당되는 코드는 위에서 언급한 컨트랙트에 포함됩니다.



수정 부분 1 : 블록 생산자의 페널티 
-------------

> 기존 문제 : 

> 해결책 : 
![슬라이드16](https://github.com/cryptogames-team/hep_contract/assets/51877962/2e037f20-eb34-4359-b604-c7770b036048)


수정 부분 2 : 보상 지급 방식 변경
-------------

기존 EOSIO에서는 블록 생산자가 블록을 다 생산하면 해당 보상이 자동으로 생산자에게 전달되지 않고 시스템 계정에 쌓이게 됩니다. 그래서 블록 생산자는 직접 cleos를 통해 보상을 수령해야되는 번거로움이 있습니다. ([해당 명령어는 cleos system claimrewards 입니다](https://developers.eos.io/manuals/eos/latest/cleos/command-reference/system/system-claimrewards)) 

그래서 Cyptogames는 이러한 불편함을 해결하고자 블록 생산자가 블록을 생산하고 확정되었을 때, 즉 라운드가 끝났을 때 보상을 자동으로 지급되도록 시스템 컨트랙트의 코드를 수정했습니다.

![슬라이드14](https://github.com/cryptogames-team/hep_contract/assets/51877962/8d3fa198-704c-4c3f-a806-07a846fb63ce)
