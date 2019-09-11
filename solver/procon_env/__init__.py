from gym.envs.registration import register
register(
    id='procon_env-v0',
    entry_point='procon_env:ProcomEnv'
)