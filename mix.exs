defmodule Mix.Tasks.Compile.CMake do
  use Mix.Task

  def run(_args) do
    {out, _} = System.cmd "mkdir", ["-p", "_build"], stderr_to_stdout: true
    IO.puts out
    {out, _} = System.cmd "cmake", ["../c_src"], cd: "_build", stderr_to_stdout: true
    IO.puts out
    :ok
  end
end

defmodule Mix.Tasks.Compile.Make do
  use Mix.Task

  def run(_args) do
    {out, _} = System.cmd("make", [], cd: "_build", stderr_to_stdout: true)
    IO.puts out
    :ok
  end
end

defmodule NatVis.Mixfile do
  use Mix.Project

  def project do
    [
      app: :nat_vis,
      version: "0.1.0",
      elixir: "~> 1.5",
      start_permanent: Mix.env == :prod,
      deps: deps(),
      compilers: [:c_make, :make] ++ Mix.compilers
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end
